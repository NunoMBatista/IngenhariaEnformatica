import flask
import logging
import psycopg2
import time
import jwt
from datetime import datetime
from flask_jwt_extended import get_jwt_identity

from global_functions import db_connection, logger, StatusCodes, check_required_fields, APPOINTMENT_DURATION, SURGERY_DURATION, payload_contains_dangerous_chars

# Check if all nurses have an id and a role
def check_nurse_fields(nurses):
    for nurse in nurses:
        if 'nurse_id' not in nurse or 'role' not in nurse:
            return False

# Check if the doctor and nurses are available in 
def check_doctor_availability(cur, doctor_id, date):
    # Check if the doctor has no simultaneous appointments
    check_doctor_appointment_availability(cur, doctor_id, date)
    
    # Check if the doctor has no simultaneous surgeries
    check_doctor_surgery_availability(cur, doctor_id, date)

def check_doctor_appointment_availability(cur, doctor_id, date):
    # Update lock (no other transaction can update appointment until this transaction is finished)
    cur.execute("""
                SELECT * FROM appointment 
                WHERE doctor_employee_contract_service_user_user_id = %s AND 
                (app_date BETWEEN %s::timestamp - INTERVAL %s AND %s::timestamp + INTERVAL %s)
                FOR UPDATE
                """, (doctor_id, date, APPOINTMENT_DURATION, date, APPOINTMENT_DURATION))
    if cur.fetchone():
        raise ValueError('Doctor is not available on the selected date')

def check_doctor_surgery_availability(cur, doctor_id, date):
    # Update lock (no other transaction can update surgery until this transaction is finished)
    cur.execute("""
                SELECT * FROM surgery 
                WHERE doctor_employee_contract_service_user_user_id = %s AND 
                (surg_date BETWEEN %s::timestamp - INTERVAL %s AND %s::timestamp + INTERVAL %s)
                FOR UPDATE
                """, (doctor_id, date, APPOINTMENT_DURATION, date, SURGERY_DURATION))
    if cur.fetchone():
        raise ValueError('Doctor is not available on the selected date')

def check_nurse_availability(cur, nurse_id, date):
    # Check if the nurse has no simultaneous appointments
    check_nurse_appointment_availability(cur, nurse_id, date)
    
    # Check if the nurse has no simultaneous surgeries
    check_nurse_surgery_availability(cur, nurse_id, date)

def check_nurse_appointment_availability(cur, nurse_id, date):
    # Update lock (no other transaction can update enrolment_appointment until this transaction is finished)
    cur.execute("""
                SELECT ap.* FROM enrolment_appointment ea
                JOIN appointment ap ON ea.appointment_app_id = ap.app_id
                WHERE ea.nurse_employee_contract_service_user_user_id = %s AND 
                (ap.app_date BETWEEN %s::timestamp - INTERVAL %s AND %s::timestamp + INTERVAL %s)
                FOR UPDATE
                """, (nurse_id, date, APPOINTMENT_DURATION, date, APPOINTMENT_DURATION)) 
    if cur.fetchone():
        raise ValueError('Nurse is not available on the selected date')

def check_nurse_surgery_availability(cur, nurse_id, date):
    # Update lock (no other transaction can update enrolment_surgery until this transaction is finished)
    cur.execute("""
                SELECT s.* FROM enrolment_surgery es
                JOIN surgery s ON es.surgery_surgery_id = s.surgery_id
                WHERE es.nurse_employee_contract_service_user_user_id = %s AND 
                (s.surg_date BETWEEN %s::timestamp - INTERVAL %s AND %s::timestamp + INTERVAL %s)
                FOR UPDATE
                """, (nurse_id, date, APPOINTMENT_DURATION, date, SURGERY_DURATION)) 
    if cur.fetchone():
        raise ValueError('Nurse is not available on the selected date')

def get_type_id(cur, type):
    cur.execute("""
                SELECT type_id FROM app_type WHERE type_name = %s
                """, (type,))
    return cur.fetchone()[0]

def insert_appointment(cur, date, doctor_id, user_id, type_id):
    cur.execute("""
                INSERT INTO appointment (app_date, doctor_employee_contract_service_user_user_id, patient_service_user_user_id, app_type_type_id) 
                VALUES (%s, %s, %s, %s) 
                RETURNING app_id
                """, (date, doctor_id, user_id, type_id))
    return cur.fetchone()[0]

def insert_nurse_app_enrolment(cur, app_id, nurses):
    for nurse in nurses:
        cur.execute("""
                    INSERT INTO enrolment_appointment (appointment_app_id, nurse_employee_contract_service_user_user_id, role_role_id) 
                    VALUES (%s, %s, (
                        SELECT role_id FROM role
                        WHERE role_name = %s
                        ))
                    """, (app_id, nurse['nurse_id'], nurse['role']))

def insert_nurse_surg_enrolment(cur, surg_id, nurses):
    for nurse in nurses:
        cur.execute("""
                    INSERT INTO enrolment_surgery (surgery_surgery_id, nurse_employee_contract_service_user_user_id, role_role_id)
                    VALUES(%s, %s, (
                        SELECT role_id FROM role
                        WHERE role_name = %s
                    ))
                    """, (surg_id, nurse['nurse_id'], nurse['role']))

def insert_hospitalization(cur, patient_id, date, nurse_id):
    cur.execute("""
                INSERT INTO hospitalization (
                                            patient_service_user_user_id, 
                                            start_date, 
                                            end_date, 
                                            nurse_employee_contract_service_user_user_id)
                VALUES(%s, %s::timestamp, %s::timestamp + INTERVAL %s, %s)
                RETURNING hosp_id
                """, (patient_id, date, date, SURGERY_DURATION, nurse_id))
    return cur.fetchone()[0]

def insert_surgery(cur, date, doctor_id, hospitalization_id, type):
    # Update the hospitalization
    cur.execute("""
                UPDATE hospitalization
                SET start_date = LEAST(start_date, %s::timestamp),
                    end_date = GREATEST(end_date, %s::timestamp + INTERVAL %s)
                WHERE hosp_id = %s;
                """, (date, date, SURGERY_DURATION, hospitalization_id))

    # Insert the surgery
    cur.execute("""
                INSERT INTO surgery (
                                    surg_date, 
                                    doctor_employee_contract_service_user_user_id, 
                                    hospitalization_hosp_id, 
                                    type)
                VALUES(%s, %s, %s, %s)
                RETURNING surgery_id
                """, (date, doctor_id, hospitalization_id, type))
    
    # Return the surgery_id
    return cur.fetchone()[0]


"""
    While scheduling an appointment, there are possible concurrency issues that need to be addressed.
    
    An update lock is used when checking the doctor and nurse availability to prevent other transactions from 
    adding appointments or surgeries to the database while the current transaction is running.
    
    In order to check if a doctor or a nurse are busy with another appointment, we check if they have any appointments APPOINTMENT_DURATION 
    before or after the selected date. Similarly, to check if a doctor or a nurse are busy with a surgery, we check if they have any
    surgeries APPONTMENT_DURATION before and SURGERY_DURATION after the selected date.
    
    Moreover, to avoid deadlocks, we need to make sure that the locks are acquired in the same order in all transactions,
    that is, first the doctor and then the nurses.
"""
def schedule_appointment():
    # Get the request payload
    payload = flask.request.get_json()
    if(payload_contains_dangerous_chars(payload)):
        response = {
            'status': StatusCodes['bad_request'],
            'errors': 'Payload contains dangerous characters'
        }
        return flask.jsonify(response)
         
    # Get the user ID from the JWT
    user_id = get_jwt_identity()
        
    # Connect to the database
    conn = db_connection()
    cur = conn.cursor()
        
    try: 
        # Start the transaction
        cur.execute("BEGIN;")
        
        # Check if all the required fields are present
        missing_keys = check_required_fields(payload, ['doctor_id', 'date', 'type'])
        if (len(missing_keys) > 0):
            response = {
                'status': StatusCodes['bad_request'],
                'errors': f'Missing required field(s): {", ".join(missing_keys)}'
            }
            return flask.jsonify(response)
      
        # Get the payload values
        doctor_id = payload['doctor_id']
        date = payload['date']
        # Nurses are optional, they have id and role
        nurses = payload.get('nurses', [])
    
        # Check if all nurses have id and role
        if(check_nurse_fields(nurses) == False):
            response = {
                'status': StatusCodes['bad_request'],
                'errors': 'Nurses must have an id and a role'
            }
            return flask.jsonify(response)

        # Check if the doctor and nurses are available
        check_doctor_availability(cur, doctor_id, date)
        for nurse in nurses:
            check_nurse_availability(cur, nurse['nurse_id'], date)
        
            
        # Get the appointment type id
        type_id = get_type_id(cur, payload['type'])
            
        # Insert the appointment
        app_id = insert_appointment(cur, date, doctor_id, user_id, type_id)
        
        # Insert the nurses into the enrolement_appointments table
        insert_nurse_app_enrolment(cur, app_id, nurses)        

        response = {
            'status': StatusCodes['success'],
            'results': f'appointment_id: {app_id}'
        }
        
    except(Exception, psycopg2.DatabaseError) as error:
        logger.error(f'POST /dbproj/appointment - error: {error}')
        response = {
            'status': StatusCodes['internal_error'],
            'errors': str(error)
        }
       
        cur.execute("ROLLBACK;")
        
    finally:
        # Commit the transaction
        cur.execute("COMMIT;")

        if cur is not None:
            cur.close()
            
    return flask.jsonify(response)


"""
    While scheduling a surgery, there are possible concurrency issues that need to be addressed.
    
    Similarly to the appointment, an update lock is used when checking the doctor and nurse availability and
    the locks are always acquired in the same order 
"""
def schedule_surgery(hospitalization_id):    
    # Get the request payload
    payload = flask.request.get_json()
    if(payload_contains_dangerous_chars(payload)):
        response = {
            'status': StatusCodes['bad_request'],
            'errors': 'Payload contains dangerous characters'
        }
        return flask.jsonify(response)
    
    logger.debug(f'POST /dbproj/surgery/{hospitalization_id} - payload: {payload}')
    
    # Connect to the database
    conn = db_connection()
    cur = conn.cursor()
    
    """
        Only if there is no hospitalization, at least one nurse is required (to be responsible for the hospitalization)
        So, extra_required_key is set to a redundant value (patient_id) if hospitalization_id is not None 
    """
    extra_required_key = 'patient_id'
    if hospitalization_id is None:
        extra_required_key = 'nurses'
    
    missing_keys = check_required_fields(payload, ['patient_id', 'doctor', extra_required_key, 'date', 'type'])
    if (len(missing_keys) > 0):
        response = {
            'status': StatusCodes['bad_request'],
            'errors': f'Missing required field(s): {", ".join(missing_keys)}'
        }
        return flask.jsonify(response)
    
    # Get the payload values
    patient_id = payload['patient_id']
    doctor_id = payload['doctor']
    date = payload['date']
         
    # At least one nurse is required for a surgery
    nurses = []
    if payload.get('nurses') is not None:
        nurses = payload.get('nurses', [])
            
        if check_nurse_fields(nurses) == False or len(nurses) == 0:
            response = {
                'status': StatusCodes['bad_request'],
                'errors': 'Nurses must have an id and a role'
            }
            return flask.jsonify(response)    

    try:
        # Start the transaction
        cur.execute("BEGIN;")

        # Check if the doctor and nurses are available
        check_doctor_availability(cur, doctor_id, date)

        # Check if the nurses are available
        for nurse in nurses:
            check_nurse_availability(cur, nurse['nurse_id'], date)
 
        # If hospitalization_id is not provided, create a new hospitalization
        if hospitalization_id is None:
            hospitalization_id = insert_hospitalization(cur, patient_id, date, nurses[0]['nurse_id'])
        else:
            # Check if the hospitalization exists
            cur.execute("""
                        SELECT * FROM hospitalization
                        WHERE hosp_id = %s AND patient_service_user_user_id = %s
                        """, (hospitalization_id, patient_id))
            if cur.fetchone() is None:
                response = {
                    'status': StatusCodes['bad_request'],
                    'errors': 'Hospitalization not found for that patient'
                }
                return flask.jsonify(response)            
            
        # Insert the surgery
        surgery_id = insert_surgery(cur, date, doctor_id, hospitalization_id, payload['type'])
               
        # Insert the nurses into the enrolement_surgery table
        insert_nurse_surg_enrolment(cur, surgery_id, nurses)

        response = {
            'status': StatusCodes['success'],
            'result': f'hospitalization_id: {hospitalization_id}, surgery_id: {surgery_id}, patient_id: {patient_id}, doctor_id: {doctor_id}, date: {date}'
        }
        
    except (Exception, psycopg2.DatabaseError) as error:
        logger.error(f'POST /dbproj/surgery/{hospitalization_id} - error: {error}')
        response = {
            'status' : StatusCodes['internal_error'],
            'errors' : str(error)
        }
        
        cur.execute("ROLLBACK;")
        
    finally:
        # Commit the transaction
        cur.execute("COMMIT;")
        
        if cur is not None:
            cur.close()
                    
    return flask.jsonify(response)    

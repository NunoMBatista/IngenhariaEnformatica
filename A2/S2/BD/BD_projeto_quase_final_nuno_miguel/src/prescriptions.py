import flask
import psycopg2
from flask_jwt_extended import get_jwt_identity, get_jwt

from global_functions import db_connection, logger, StatusCodes, check_required_fields, payload_contains_dangerous_chars

def check_medication_fields(medications):
    for medication in medications:
        if 'medicine' not in medication or 'posology_dose' not in medication or 'posology_frequency' not in medication:
            return False
    return True

def prescribe_medication():
    commit_success = False
    # Get the payload data
    payload = flask.request.get_json()
    if(payload_contains_dangerous_chars(payload)):
        response = {
            'status': StatusCodes['bad_request'],
            'errors': 'Payload contains dangerous characters'
        }
        return flask.jsonify(response)
    
    logger.debug(f'POST /dbproj/register/service_user - payload: {payload}')
    
    # Check if the required fields are present
    missing_keys = check_required_fields(payload, ['type', 'event_id', 'validity', 'medicines'])
    if len(missing_keys) > 0:
        response = {
            'status': StatusCodes['bad_request'],
            'errors': f'Missing required field(s): {", ".join(missing_keys)}'
        }
        return flask.jsonify(response)
    
    # Check if the medications are valid
    if not check_medication_fields(payload['medicines']):
        response = {
            'status': StatusCodes['bad_request'],
            'errors': 'Invalid medication fields'
        }
        return flask.jsonify(response)
    
    # Connect to the database
    conn = db_connection()
    cur = conn.cursor()
    
    try:
        cur.execute("BEGIN;")
        
        # Insert into prescription
        cur.execute("""
                    INSERT INTO prescription (validity)
                    VALUES (%s)
                    RETURNING presc_id
                    """, (payload['validity'],))
        presc_id = cur.fetchone()[0]
        
        for medication in payload['medicines']:
            # Use a subquery to get med_id
            cur.execute("""
                        INSERT INTO dose (prescription_presc_id, medication_med_id, amount, time_of_day)
                        SELECT %s, med_id, %s, %s
                        FROM medication WHERE med_name = %s
                        """, (presc_id, medication['posology_dose'], medication['posology_frequency'], medication['medicine']))
               
        if payload['type'].lower() == 'appointment':
            cur.execute("""
                        INSERT INTO prescription_appointment (prescription_presc_id, appointment_app_id)
                        VALUES (%s, %s)
                        """, (presc_id, payload['event_id']))
        elif payload['type'].lower() == 'hospitalization':
            cur.execute("""
                        INSERT INTO hospitalization_prescription (prescription_presc_id, hospitalization_hosp_id)
                        VALUES (%s, %s)
                        """, (presc_id, payload['event_id'])) 
        else:
            response = {
                'status': StatusCodes['bad_request'],
                'errors': 'Invalid type'
            }
            return flask.jsonify(response)
              
        response = {
            'status': StatusCodes['success'],
            'errors': None,
            'results': f'prescription_id: {presc_id}, type: {payload["type"]}, event_id: {payload["event_id"]}'
        }
      
        cur.execute("COMMIT;")
        commit_success = True
    
    except (Exception, psycopg2.DatabaseError) as error:
        cur.execute("ROLLBACK;")
        logger.error(f'POST /dbproj/prescriptions - {error}')
        response = {
            'status': StatusCodes['internal_error'],
            'errors': str(error)
        }
        print("final")
        return flask.jsonify(response)
    
    finally:        
        if not commit_success:
            cur.execute("ROLLBACK;")
        
        if conn is not None:
            conn.close()
               
        return flask.jsonify(response)
    
def get_prescriptions(user_id):
    # Write to debug log
    logger.debug(f'GET /dbproj/prescriptions/{user_id}')
    
    if (get_jwt_identity() != user_id) and (get_jwt()['role'] == 'patient'):
        response = {
            'status': StatusCodes['bad_request'],
            'errors': 'You can only view your own prescriptions.'
        }
        return flask.jsonify(response)
    
    # Connect to the database
    conn = db_connection()
    cur = conn.cursor(cursor_factory=psycopg2.extras.RealDictCursor)
    
    try:
        with open('queries/get_prescriptions.sql', 'r') as f:
            query = f.read()
            cur.execute(query, (user_id,))
                
        prescriptions = cur.fetchall()
        response = {
            'status': StatusCodes['success'],
            'errors': None,
            'prescriptions': prescriptions
        }
        
    except (Exception, psycopg2.DatabaseError) as error:
        logger.error(error)
        response = {
            'status': StatusCodes['internal_error'],
            'errors': str(error)
        }
        return flask.jsonify(response)
        
    finally:
        cur.execute('COMMIT;')
        if conn is not None:
            conn.close()
        return flask.jsonify(response)
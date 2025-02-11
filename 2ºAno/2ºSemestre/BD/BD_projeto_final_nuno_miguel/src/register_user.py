import flask
import psycopg2

from global_functions import db_connection, logger, StatusCodes, check_required_fields, payload_contains_dangerous_chars
from hashing import hash_password

def insert_service_user(cur, name, nationality, phone, birthday, email, password):
    cur.execute("""
                INSERT INTO service_user (name, nationality, phone, birthday, email, password)
                VALUES (%s, %s, %s, %s, %s, %s)
                RETURNING user_id;
                """, (name, nationality, phone, birthday, email, password))
    
    # Get and return the user_id
    user_id = cur.fetchone()[0]
    return user_id
    
def insert_patient(cur, user_id):
    cur.execute("""
                INSERT INTO patient (service_user_user_id)
                VALUES (%s);
                """, (str(user_id),))
    
def insert_employee(cur, user_id, start_date, end_date):
    cur.execute("""
                INSERT INTO employee_contract (service_user_user_id, contract_start_date, contract_end_date)
                VALUES (%s, %s, %s)
                RETURNING contract_contract_id;
                """, (user_id, start_date, end_date))
        
    contract_id = cur.fetchone()[0]
    return contract_id

def insert_assistant(cur, user_id):
    cur.execute("""
                INSERT INTO assistant (employee_contract_service_user_user_id)
                VALUES (%s);
                """, (str(user_id),))
    
def insert_nurse(cur, user_id, rank_id):
    cur.execute("""
                INSERT INTO nurse (employee_contract_service_user_user_id, rank_rank_id)
                VALUES (%s, %s);
                """, (str(user_id), str(rank_id)))
    
def insert_doctor(cur, user_id, university, graduation_date, specializations):
    cur.execute("""
                INSERT INTO doctor (employee_contract_service_user_user_id, university, graduation_date)
                VALUES (%s, %s, %s);
                """, (str(user_id), university, graduation_date))
   
    for spec_name in specializations:
        cur.execute("""
                    SELECT spec_id
                    FROM specialization
                    WHERE spec_name = %s;
                    """, (spec_name,))
        spec_id = cur.fetchone()
        if spec_id is None:
            raise Exception(f'Specialization {spec_name} does not exist')
        spec_id = spec_id[0]
        
        cur.execute("""
                    INSERT INTO specialization_doctor (specialization_spec_id, doctor_employee_contract_service_user_user_id)
                    VALUES (%s, %s);
                    """, (str(spec_id), str(user_id)))
        

def register_service_user(user_type, extra_fields):
    commit_success = False
    
    # Get the request payload
    payload = flask.request.get_json()
    if(payload_contains_dangerous_chars(payload)):
        response = {
            'status': StatusCodes['bad_request'],
            'errors': 'Payload contains dangerous characters'
        }
        return flask.jsonify(response)
    
    # Connect to the database
    conn = db_connection()
    cur = conn.cursor()
    
    # Write request to debug log
    logger.debug(f'POST /dbproj/register/service_user - payload: {payload}')
    
    # Check if all required fields are present
    required_fields = ['name', 'nationality', 'phone', 'birthday', 'email', 'password']
    missing_keys = check_required_fields(payload, required_fields + extra_fields)
    if(len(missing_keys) > 0):
        response = {
            'status': StatusCodes['bad_request'],
            'errors': f'Missing required field(s): {", ".join(missing_keys)}'
        }
        return flask.jsonify(response)
    
    try:
        # Start the transaction
        cur.execute('BEGIN;')
        
        # Insert the user into the service_user table
        user_id = insert_service_user(cur, payload['name'], payload['nationality'], str(payload['phone']), payload['birthday'], payload['email'], hash_password(payload['password']))
        
        # Insert the user into the specific table
        if(user_type == 'patient'):
            insert_patient(cur, user_id)
        else:
            contract_id = insert_employee(cur, user_id, payload['contract_start_date'], payload['contract_end_date'])
            
            if(user_type == 'assistant'):
                insert_assistant(cur, user_id)
            
            if(user_type == 'nurse'):
                insert_nurse(cur, user_id, payload['rank_id'])
                
            if(user_type == 'doctor'):
                insert_doctor(cur, user_id, payload['university'], payload['graduation_date'], payload['specializations'])
                
        # Commit the transaction
        cur.execute('COMMIT;')
        commit_success = True

        response = {
            'status': StatusCodes['success'],
            'errors': None,
            'results': f'user_id: {user_id}'
        }
        
    except(Exception, psycopg2.DatabaseError) as error:
        logger.error(f'POST /dbproj/register/{user_type} - error: {error}')
        response = {
            'status': StatusCodes['internal_error'],
            'errors': str(error)
        }
        cur.execute('ROLLBACK;')
        
    finally:
        if(commit_success == False):
            cur.execute('ROLLBACK;')
        
        if cur is not None:
            cur.close()
        
        if conn is not None:
            conn.close()
            
        return response
             
def register_patient():
    return register_service_user('patient', [])

def register_assistant():
    return register_service_user('assistant', ['contract_start_date', 'contract_end_date'])

def register_nurse():
    return register_service_user('nurse', ['contract_start_date', 'contract_end_date', 'rank_id'])

def register_doctor():
    return register_service_user('doctor', ['contract_start_date', 'contract_end_date', 'university', 'graduation_date', 'specializations'])    
    
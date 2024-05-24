import flask
import psycopg2
from flask_jwt_extended import create_access_token, get_jwt, set_access_cookies
from functools import wraps

from global_functions import db_connection, logger, StatusCodes, check_required_fields, payload_contains_dangerous_chars
from hashing import verify_password

# Authenticate a user and return an access token
def authenticate_user():
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
    logger.debug(f'POST /dbproj/user - payload: {payload}')
    
    # Check if all required fields are present
    missing_keys = check_required_fields(payload, ['email', 'password'])
    if (len(missing_keys) > 0):
        response = {
            'status': StatusCodes['bad_request'],
            'errors': f'Missing required field(s): {", ".join(missing_keys)}'
        }
        return flask.jsonify(response)        
    
    try:
        cur.execute('BEGIN;')
        
        # Query the database for the user_id
        cur.execute("""
                    SELECT user_id, password FROM service_user
                    WHERE email = %s 
                    """, (payload['email'],))
        query_return = cur.fetchone()   
        
        if query_return is None:
            # Return an error response
            response = {
                'status': StatusCodes['bad_request'],
                'errors': 'User not found'
            }
            return flask.jsonify(response)
                
        user_id = query_return[0]
        stored_password = query_return[1]

        # Verify the password against the stored hash
        if not verify_password(stored_password, payload['password']):
            logger.error('Invalid password was provided')
            # Return an error response
            response = {
                'status': StatusCodes['bad_request'],
                'errors': 'Invalid password'
            }
            return flask.jsonify(response)
        

        # Determine the user's role
        role = None 
        tables = {
            'patient': 'service_user_user_id',
            'assistant': 'employee_contract_service_user_user_id',
            'nurse': 'employee_contract_service_user_user_id',
            'doctor': 'employee_contract_service_user_user_id'
        }
        # Query each possible role table to see if the user_id is present
        for table, id_column in tables.items():
            query = f"""
                    SELECT 1 FROM {table}
                    WHERE {id_column} = %s 
                    """
            cur.execute(query, (user_id,))
            # If the user_id is present in the table, the user has that role
            if cur.fetchone() is not None:
                role = table
                break
            
        logger.debug(f'POST /dbproj/user - user_id: {user_id}, role: {role}')
    
        # Generate an access token
        additional_claims = {'role': role}
        access_token = create_access_token(identity=user_id, additional_claims=additional_claims)
        
        # Create a Flask Response object in order to set the access token as a cookie
        response = flask.make_response(flask.jsonify({
            'status': StatusCodes['success'],
            'errors': None,
            'results': access_token
        }))
        
        # Set the access token as a cookie 
        set_access_cookies(response, access_token)
        
        cur.execute('COMMIT;')
                
    except(Exception, psycopg2.DatabaseError) as error:
        logger.error(f'POST /dbproj/user - error: {error}')
        response = flask.make_response(flask.jsonify({
            'status': StatusCodes['internal_error'],
            'errors': str(error)
        }))
               
        return flask.jsonify(response)
 
    finally:
        if conn is not None:
            conn.close()
    
    return response
    
# Decorator to check if the user has the required role
def role_required(required_roles):
    def decorator(func):
        @wraps(func)
        def wrapper(*args, **kwargs): # Same arguments as the original function
            claims = get_jwt() 
            # Check if the user has the required role
            if claims['role'] not in required_roles: 
                logger.error(f'Insufficient permissions - required: {required_roles}, actual: {claims["role"]}')
                response = {
                    'status': StatusCodes['bad_request'],
                    'errors': f'Insufficient permissions - required: {required_roles}, actual: {claims["role"]}'
                }
                return flask.jsonify(response)
            # If the user has the required role, call the original function
            return func(*args, **kwargs)
        # Return the wrapper function to be called instead of the original function
        return wrapper
    # Return the decorator function to be called with the required role
    return decorator

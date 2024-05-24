import flask
import psycopg2
from flask_jwt_extended import get_jwt_identity

from global_functions import db_connection, logger, StatusCodes, check_required_fields, payload_contains_dangerous_chars

def execute_payment(bill_id):
    commit_success = False
        
    # Get the request payload
    payload = flask.request.get_json()
    if(payload_contains_dangerous_chars(payload)):
        response = {
            'status': StatusCodes['bad_request'],
            'errors': 'Payload contains dangerous characters'
        }
        return flask.jsonify(response)
    
    # Write request to debug log
    logger.debug(f'POST /dbproj/dbproj/bills/{bill_id} - payload: {payload}')
        
    # Get the user ID from the JWT
    user_id = get_jwt_identity()
        
    # Connect to the database
    conn = db_connection()
    cur = conn.cursor()
    
    missing_keys = check_required_fields(payload, ['amount', 'payment_method'])
    if (len(missing_keys) > 0):
        logger.error(f'POST /dbproj/dbproj/bills/{bill_id} - error: Missing required field(s): {", ".join(missing_keys)}')
        response = {
            'status': StatusCodes['bad_request'],
            'errors': f'Missing required field(s): {", ".join(missing_keys)}'
        }
        return flask.jsonify(response)
    
    # Get the payload values
    amount = payload['amount']
    payment_method = payload['payment_method']
    
    # Check if the bill is associated with the user
    cur.execute("""
                SELECT patient_service_user_user_id 
                FROM appointment 
                WHERE bill_bill_id = %s 
                
                UNION
                
                SELECT patient_service_user_user_id 
                FROM hospitalization 
                WHERE bill_bill_id = %s
                """, (bill_id, bill_id))
    result = cur.fetchone()
    if result is None or result[0] != user_id:
        logger.error(f'POST /dbproj/dbproj/bills/{bill_id} - error: Bill does not belong to the patient')
        response = {
            'status': StatusCodes['bad_request'],
            'errors': 'You can only pay your own bills.'
        }
        return flask.jsonify(response)
       
    try:
        # Start the transaction
        cur.execute('BEGIN;')

        # Process the payment
        cur.execute("""
                    INSERT INTO payment (bill_bill_id, amount, payment_method, payment_date) 
                    VALUES (%s, %s, %s, NOW())
                    """, (bill_id, amount, payment_method))

        """
            A trigger will now take place to update the bill 
            according to the payment made
        """

        # Get the remaining cost of the bill
        cur.execute("""
                    SELECT (total_cost - already_paid) as remaining_cost  
                    FROM bill 
                    WHERE bill_id = %s
                    """, (bill_id,))
        remaining_cost = cur.fetchone()[0]
        
        # Commit the transaction
        cur.execute('COMMIT;')
        commit_success = True

        response = {
            'status': StatusCodes['success'], 
            'errors': None,                    
            'results': f'remaining: {remaining_cost}'}
        
    except(Exception, psycopg2.DatabaseError) as error:
        if 'excessive_payment' in str(error):
            logger.error(f'POST /dbproj/dbproj/bills/{bill_id} - error: {error}')
            response = {'status': StatusCodes['bad_request'], 'errors': 'you paid more than what you owe'}
        else:
            logger.error(f'POST /dbproj/dbproj/bills/{bill_id} - error: {error}')
            response = {'status': StatusCodes['internal_error'], 'errors': str(error)}
 

        # Rollback the transaction
        cur.execute('ROLLBACK;')
    
    finally:
        if not commit_success:
            cur.execute('ROLLBACK;')
        
        if conn is not None:
            conn.close()
        
        return flask.jsonify(response)   
    

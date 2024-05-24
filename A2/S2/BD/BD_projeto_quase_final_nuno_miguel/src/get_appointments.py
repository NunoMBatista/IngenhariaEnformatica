import flask
import psycopg2
from flask_jwt_extended import get_jwt_identity, get_jwt
from psycopg2.extras import RealDictCursor


from global_functions import db_connection, logger, StatusCodes

def get_appointments(user_id):
    # A patient can only view their own appointments
    if (get_jwt_identity() != user_id) and (get_jwt()['role'] == 'patient'):
        response = {
            'status': StatusCodes['bad_request'],
            'errors': 'You can only view your own appointments.'
        }
        return flask.jsonify(response)
    
    # Connect to the database
    conn = db_connection()
    cur = conn.cursor(cursor_factory=RealDictCursor)

    # Write to debug log
    logger.debug(f'POST /dbproj/appointments/{user_id}')

    try:
        # Get the appointments
        cur.execute("""
                    SELECT app_id AS id, app_date AS date, doctor_employee_contract_service_user_user_id AS doctor_id
                    FROM appointment 
                    WHERE patient_service_user_user_id = %s
                    """, (user_id,))
        
        appointments = cur.fetchall()
        
        if len(appointments) == 0:
            response = {
                'status': StatusCodes['not_found'],
                'errors': 'No appointments found.'
            }
            return flask.jsonify(response)
        
        response = {
            'status': StatusCodes['success'],
            'errors': None,
            'appointments': appointments
        }

    except (Exception, psycopg2.DatabaseError) as error:
        logger.error(error)
        response = {
            'status': StatusCodes['internal_error'],
            'errors': 'An error occurred while fetching the appointments.'
        }
        return flask.jsonify(response)
    
    finally:
        cur.close()
        conn.close()
        
    return flask.jsonify(response)
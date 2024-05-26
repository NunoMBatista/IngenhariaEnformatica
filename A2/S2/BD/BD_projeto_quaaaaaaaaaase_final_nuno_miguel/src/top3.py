import flask
import psycopg2
from psycopg2.extras import RealDictCursor

from global_functions import db_connection, logger, StatusCodes

def top3():
    # Write to debug log
    logger.debug(f'GET /dbproj/top3')
    
    # Connect to the database
    conn = db_connection()
    cur = conn.cursor(cursor_factory=RealDictCursor)
    
    try:       
        with open('queries/top3.sql', 'r') as f:
            query = f.read()
            cur.execute(query)
            top3_list = cur.fetchall()     
            cur.execute('COMMIT;')
            response = {
                'status': StatusCodes['success'],
                'errors': None,
                'response': top3_list
            }

    except (Exception, psycopg2.DatabaseError) as error:
        logger.error(f'GET /dbproj/top3 - {error}')
        response = {
            'status': StatusCodes['internal_error'],
            'errors': str(error)
        }
        return flask.jsonify(response)
    
    finally:
        if cur is not None:
            cur.close()
        
        if conn is not None:
            conn.close()
            
        return flask.jsonify(response)
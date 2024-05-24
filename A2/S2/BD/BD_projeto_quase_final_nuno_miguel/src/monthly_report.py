import flask
import psycopg2
from psycopg2.extras import RealDictCursor

from global_functions import db_connection, logger, StatusCodes

def monthly_report():
    # Write to debug log
    logger.debug(f'GET /report')
    
    # Connect to the database
    conn = db_connection()
    cur = conn.cursor(cursor_factory=RealDictCursor)
    
    try:
        with open('queries/monthly_report.sql', 'r') as f:
            query = f.read()
            cur.execute(query)
        
        summary = cur.fetchall()
        response = {
            'status': StatusCodes['success'],
            'errors': None,
            'response': summary
        }
        
    except (Exception, psycopg2.DatabaseError) as error:
        logger.error(f'GET /report - {error}')
        response = {
            'status': StatusCodes['internal_error'],
            'errors': str(error)
        }
        return flask.jsonify(response)
    
    finally:
        if conn is not None:
            conn.close()
            
        return flask.jsonify(response)
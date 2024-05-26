import logging
import psycopg2


"""
    Global variabless
"""

db_connection = None
logger = logging.getLogger('logger')
StatusCodes = {
    'bad_request': 400,
    'success': 200,
    'internal_error': 500
}

APPOINTMENT_DURATION = '30 minutes'
SURGERY_DURATION = '2 hours'

"""
    Global functions
"""

# Connect to the database
def db_connection():
    try:
        db = psycopg2.connect(
            user = 'hms_admin',
            password = 'hms_admin',
            host = '127.0.0.1',
            port = '5432',
            database = 'hms_db'
        )
        return db
    except (Exception, psycopg2.DatabaseError) as error:
        logger.error(f'Error connecting to the database: {error}')
        return None

# Run a SQL script
def run_sql_script(script):
    conn = db_connection()
    cursor = conn.cursor()
    
    try:
        with open(script, 'r') as f:
            script_str = f.read()
            cursor.execute(script_str)
        print(script + ' executed successfully')
        conn.commit()
    except Exception as e:
        conn.rollback()
        raise e
    finally:
        cursor.close()
        conn.close()

# Landing page
def landing_page():
    return """
    <!DOCTYPE html>
    <html>
    <head>
        <style>
            body {
                background-color: #f2f2f2;
                font-family: Arial, sans-serif;
            }
            .container {
                width: 80%;
                margin: auto;
                text-align: center;
            }
            h1 {
                color: #333;
            }
            p {
                font-size: 1.2em;
            }
        </style>
    </head>
    <body>
        <div class="container">
            <h1>Welcome to the Hospital Management System</h1>
        </div>
    </body>
    </html>
    """
    
# Check required fields in a payload
def check_required_fields(payload, required_keys):
    missing_keys = [key for key in required_keys if key not in payload]
    if len(missing_keys) > 0:
        logger.error(f'Missing required fields: {", ".join(missing_keys)}')
    return missing_keys

# Search dangerous characters in a user input string
def string_contains_dangerous_chars(input_str):
    # Check for SQL injection characters
    dangerous_chars = [';', '--', '/*', '*/'] 
    for char in dangerous_chars:
        if char in input_str:
            return True
    return False

# Check if a payload contains dangerous characters
def payload_contains_dangerous_chars(payload):
    for key, value in payload.items():
        # If it's a dictionary, recursively call the function
        if isinstance(value, dict):
            if payload_contains_dangerous_chars(value):
                return True
        # If it's a list or tuple, iterate over the elements and check each one
        elif isinstance(value, (list, tuple)):
            for element in value:
                if isinstance(element, str) and string_contains_dangerous_chars(element):
                    return True
                elif isinstance(element, (dict, list, tuple)) and payload_contains_dangerous_chars({0: element}):
                    return True
        # If it's a string, check for dangerous characters
        elif isinstance(value, str):
            if string_contains_dangerous_chars(value):
                return True
    return False

# Setup/reset database
def setup_database():
    print("Setting up the database")
    
    # Disconnect all users
    run_sql_script('queries/disconnect_users.sql')

    # Remove every table if they exist
    run_sql_script('queries/drop_tables.sql')

    # Create the tables and add indexing
    run_sql_script('queries/tables_creation.sql')
    
    # Create the constraints
    run_sql_script('queries/tables_constraints.sql')
    
    # Populate the tables
    run_sql_script('queries/populate_tables.sql')
    
    # Create the triggers
    run_sql_script('queries/create_bill_triggers.sql')
    
# Check if the database is properly setup
def check_database_setup():
    # Check if the database is set up
    try:
        conn = db_connection()
        cur = conn.cursor()            
        with open('queries/check_setup.sql', 'r') as f:
            query = f.read()
            
            cur.execute(query)
            result = cur.fetchall()
            
            # Check if any of the tables are not present
            for row in result:
                if False in row:
                    raise Exception('The database is not properly set up. Please run the script with the --setup flag')
            
    
    except Exception as e:  
        print(str(e))
        exit(1)
    
    finally:
        cur.close()
        conn.close()
    
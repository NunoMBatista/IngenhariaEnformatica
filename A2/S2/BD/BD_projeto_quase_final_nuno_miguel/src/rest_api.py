'''
               Databases
             LEI 2023/2024

     +----------------------------+
     | Hospital Management System |
     +----------------------------+

     -> Nuno Batista 👍 2022213951
    -> Miguel Martins 👍 2022213951
    
'''

import flask
import logging
import psycopg2
import time
import jwt
from datetime import timedelta
from flask_jwt_extended import JWTManager, create_access_token, get_jwt_identity, jwt_required


from config import Config
# Import global functions and variables
from global_functions import db_connection, logger, StatusCodes, run_sql_script, landing_page, check_required_fields
# Import the register functions
from register_user import register_patient, register_assistant, register_nurse, register_doctor
# Import the authentication function
from authentication import authenticate_user, role_required
# Import the scheduling function
from scheduling import schedule_appointment, schedule_surgery
# Import the payment function
from payment import execute_payment
# Import the get appointments function
from get_appointments import get_appointments
# Import the prescription function
from prescriptions import prescribe_medication, get_prescriptions
# Import the top3 function
from top3 import top3
# Import the daily summary function
from daily_summary import daily_summary
# Import the monthly report function
from monthly_report import monthly_report

app = flask.Flask(__name__)
app.config.from_object(Config)
jwt = JWTManager(app)

# Improve later
app.route('/') (landing_page)

# Register a patient
@app.route('/dbproj/register/patient', methods=['POST'])
def register_patient_endpoint():
    return register_patient()

# Register an assistant
@app.route('/dbproj/register/assistant', methods=['POST'])
def register_assistant_endpoint():
    return register_assistant()

# Register a nurse
@app.route('/dbproj/register/nurse', methods=['POST'])
def register_nurse_endpoint():
    return register_nurse()

# Register a doctor
@app.route('/dbproj/register/doctor', methods=['POST'])
def register_doctor_endpoint():
    return register_doctor()

# Authenticate a user
@app.route('/dbproj/user', methods=['PUT'])
def authenticate_user_endpoint():
    return authenticate_user()

@app.route('/dbproj/appointment', methods=['POST'])
@jwt_required()
@role_required('patient')
def schedule_appointment_endpoint():
    return schedule_appointment()

@app.route('/dbproj/appointments/<int:user_id>', methods=['GET'])
@jwt_required()
@role_required(['patient', 'doctor'])
def get_appointments_endpoint(user_id):
    return get_appointments(user_id)
    
@app.route('/dbproj/surgery', methods=['POST'])
@app.route('/dbproj/surgery/<int:hospitalization_id>', methods=['POST'])
@jwt_required()
@role_required('assistant')
def schedule_surgery_endpoint(hospitalization_id=None):
    return schedule_surgery(hospitalization_id)

@app.route('/dbproj/prescription', methods=['POST'])
@jwt_required()
@role_required('doctor')
def prescribe_medication_endpoint():
    return prescribe_medication()

@app.route('/dbproj/prescriptions/<int:user_id>', methods=['GET'])
@jwt_required()
@role_required(['patient', 'doctor', 'nurse', 'assistant'])
def get_prescriptions_endpoint(user_id):
    return get_prescriptions(user_id)

@app.route('/dbproj/bills/<int:bill_id>', methods=['POST'])
@jwt_required()
@role_required('patient')
def execute_payment_endpoint(bill_id): 
    return execute_payment(bill_id) 

@app.route('/dbproj/top3', methods=['GET'])
@jwt_required()
@role_required('assistant')
def top3_endpoint():
    return top3()

@app.route('/dbproj/daily/<date_str>', methods=['GET'])
@jwt_required()
@role_required('assistant')
def daily_summary_endpoint(date_str):
    return daily_summary(date_str)

@app.route('/dbproj/report', methods=['GET'])
@jwt_required()
@role_required('assistant')
def monthly_report_endpoint():
    return monthly_report()

if __name__ == '__main__':
    # Set up logging
    logging.basicConfig(filename='log_file.log')   
    logger.setLevel(logging.DEBUG)
    ch = logging.StreamHandler()
    ch.setLevel(logging.DEBUG)
    
    # Set up the formatter
    formatter = logging.Formatter('%(asctime)s [%(levelname)s]: %(message)s', '%H:%M:%S')
    ch.setFormatter(formatter)
    logger.addHandler(ch)
    
    # Run table creation script
    # run_sql_script('queries/drop_tables.sql')
    # run_sql_script('queries/tables_creation.sql')
    # run_sql_script('queries/tables_constraints.sql')
    # run_sql_script('queries/populate_tables.sql')
    # run_sql_script('queries/create_bill_triggers.sql')
        
    
    # Start the server
    host = '127.0.0.1'
    port = 8080
    logger.info(f'Starting HMS server on http://{host}:{port}')
    app.run(host=host, threaded=True, port=port)
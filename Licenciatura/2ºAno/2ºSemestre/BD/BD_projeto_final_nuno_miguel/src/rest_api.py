'''
               Databases
             LEI 2023/2024

     +----------------------------+
     | Hospital Management System |
     +----------------------------+

     -> Nuno Batista 👍 2022213951
    -> Miguel Martins 👍 2022213951
    
    
    
    Execution:
        $ <python interpreter> rest_api.py [flags]
    
    Flags: 
        --setup: Setup/reset the database
'''

import argparse
import flask
import logging
from flask_jwt_extended import JWTManager, jwt_required


from config import Config
# Import global functions and variables
from global_functions import logger, landing_page, setup_database, check_database_setup
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

# Schedule an appointment
@app.route('/dbproj/appointment', methods=['POST'])
@jwt_required()
@role_required('patient')
def schedule_appointment_endpoint():
    return schedule_appointment()

# Get user's appointments
@app.route('/dbproj/appointments/<int:user_id>', methods=['GET'])
@jwt_required()
@role_required(['patient', 'doctor'])
def get_appointments_endpoint(user_id):
    return get_appointments(user_id)
    
# Schedule a surgery
@app.route('/dbproj/surgery', methods=['POST'])
@app.route('/dbproj/surgery/<int:hospitalization_id>', methods=['POST'])
@jwt_required()
@role_required('assistant')
def schedule_surgery_endpoint(hospitalization_id=None):
    return schedule_surgery(hospitalization_id)

# Prescribe medication
@app.route('/dbproj/prescription', methods=['POST'])
@jwt_required()
@role_required('doctor')
def prescribe_medication_endpoint():
    return prescribe_medication()

# Get user's prescriptions
@app.route('/dbproj/prescriptions/<int:user_id>', methods=['GET'])
@jwt_required()
@role_required(['patient', 'doctor', 'nurse', 'assistant'])
def get_prescriptions_endpoint(user_id):
    return get_prescriptions(user_id)

# Execute payment
@app.route('/dbproj/bills/<int:bill_id>', methods=['POST'])
@jwt_required()
@role_required('patient')
def execute_payment_endpoint(bill_id): 
    return execute_payment(bill_id) 

# Top 3 user's for the last 30 days
@app.route('/dbproj/top3', methods=['GET'])
@jwt_required()
@role_required('assistant')
def top3_endpoint():
    return top3()

# Daily summary for a given date
@app.route('/dbproj/daily/<date_str>', methods=['GET'])
@jwt_required()
@role_required('assistant')
def daily_summary_endpoint(date_str):
    return daily_summary(date_str)

# Doctor's with most surgeries per month for the last year
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

    # Parse the arguments
    parser = argparse.ArgumentParser(description='Hospital Management System')
    parser.add_argument('--setup',
                        action='store_true',
                        help='Setup the database')
    
    args = parser.parse_args()
    
    if args.setup:
        # Setup the database
        setup_database()

    # Check if the database is set up
    logger.info('Checking if the database is set up')
    with app.app_context():
        try:
            check_database_setup()
        except Exception as e:
            print(str(e))
            exit(1)

    # Start the server
    host = '127.0.0.1'
    port = 8080
    logger.info(f'Starting HMS server on http://{host}:{port}')
    app.run(host=host, threaded=True, port=port)
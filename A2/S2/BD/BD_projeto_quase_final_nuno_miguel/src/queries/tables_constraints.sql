ALTER TABLE service_user ADD UNIQUE (phone);
ALTER TABLE service_user ADD UNIQUE (email);
ALTER TABLE service_user ADD CONSTRAINT constraint_name CHECK (--  'name' field only contains letters and spaces
name ~ '^[a-zA-Z ]+$');
ALTER TABLE service_user ADD CONSTRAINT constraint_phone CHECK (--  phone - Phone number has to comprise of 9 digits
LENGTH(CAST(phone AS text)) = 9 AND CAST(phone AS text) ~ '^[0-9]+$');
ALTER TABLE service_user ADD CONSTRAINT constraint_birthday CHECK (-- birthday - Birthday can't be earlier than 1904
birthday > '1904-01-01'
);
ALTER TABLE service_user ADD CONSTRAINT constraint_email CHECK (-- email - Email has to be in the format of '<string>@<string>.<string (len>2))>'
email ~ '^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$');
ALTER TABLE service_user ADD CONSTRAINT constraint_password CHECK (-- password - Password has to be at least 4 characters long
LENGTH(password) >= 4);
ALTER TABLE employee_contract ADD UNIQUE (contract_contract_id);
ALTER TABLE employee_contract ADD CONSTRAINT employee_contract_fk1 FOREIGN KEY (service_user_user_id) REFERENCES service_user(user_id);
ALTER TABLE assistant ADD CONSTRAINT assistant_fk1 FOREIGN KEY (employee_contract_service_user_user_id) REFERENCES employee_contract(service_user_user_id);
ALTER TABLE nurse ADD CONSTRAINT nurse_fk1 FOREIGN KEY (rank_rank_id) REFERENCES rank(rank_id);
ALTER TABLE nurse ADD CONSTRAINT nurse_fk2 FOREIGN KEY (employee_contract_service_user_user_id) REFERENCES employee_contract(service_user_user_id);
ALTER TABLE doctor ADD UNIQUE (licence_id);
ALTER TABLE doctor ADD CONSTRAINT doctor_fk1 FOREIGN KEY (employee_contract_service_user_user_id) REFERENCES employee_contract(service_user_user_id);
ALTER TABLE doctor ADD CONSTRAINT constraint_0 CHECK (--  'university' field only contains letters and spaces
university~ '^[a-zA-Z ]+$');
ALTER TABLE patient ADD CONSTRAINT patient_fk1 FOREIGN KEY (service_user_user_id) REFERENCES service_user(user_id);
ALTER TABLE appointment ADD UNIQUE (bill_bill_id);
ALTER TABLE appointment ADD CONSTRAINT appointment_fk1 FOREIGN KEY (app_type_type_id) REFERENCES app_type(type_id);
ALTER TABLE appointment ADD CONSTRAINT appointment_fk2 FOREIGN KEY (bill_bill_id) REFERENCES bill(bill_id);
ALTER TABLE appointment ADD CONSTRAINT appointment_fk3 FOREIGN KEY (patient_service_user_user_id) REFERENCES patient(service_user_user_id);
ALTER TABLE appointment ADD CONSTRAINT appointment_fk4 FOREIGN KEY (doctor_employee_contract_service_user_user_id) REFERENCES doctor(employee_contract_service_user_user_id);
ALTER TABLE surgery ADD CONSTRAINT surgery_fk1 FOREIGN KEY (hospitalization_hosp_id) REFERENCES hospitalization(hosp_id);
ALTER TABLE surgery ADD CONSTRAINT surgery_fk2 FOREIGN KEY (doctor_employee_contract_service_user_user_id) REFERENCES doctor(employee_contract_service_user_user_id);
ALTER TABLE surgery ADD CONSTRAINT constraint_1 CHECK (--  'type' field only contains letters and spaces
type~ '^[a-zA-Z ]+$');
ALTER TABLE hospitalization ADD UNIQUE (bill_bill_id);
ALTER TABLE hospitalization ADD CONSTRAINT hospitalization_fk1 FOREIGN KEY (bill_bill_id) REFERENCES bill(bill_id);
ALTER TABLE hospitalization ADD CONSTRAINT hospitalization_fk2 FOREIGN KEY (patient_service_user_user_id) REFERENCES patient(service_user_user_id);
ALTER TABLE hospitalization ADD CONSTRAINT hospitalization_fk3 FOREIGN KEY (nurse_employee_contract_service_user_user_id) REFERENCES nurse(employee_contract_service_user_user_id);

ALTER TABLE hospitalization ADD CONSTRAINT constraint_1 CHECK (-- start_date must be previous to end_date
end_date >= start_date);
ALTER TABLE medication ADD UNIQUE (med_name);
ALTER TABLE medication ADD CONSTRAINT constraint_0 CHECK (--  'med_name' field only contains letters and spaces
med_name ~ '^[a-zA-Z ]+$');
ALTER TABLE side_effect ADD UNIQUE (symptom);
ALTER TABLE side_effect ADD CONSTRAINT constraint_0 CHECK (--  'symptom' field only contains letters and spaces
symptom~ '^[a-zA-Z ]+$');
ALTER TABLE bill ADD CONSTRAINT total_cost_check CHECK (-- 'total_cost' must be positive
total_cost >= 0);
ALTER TABLE bill ADD CONSTRAINT already_paid_check CHECK (-- 'already_paid' must be positive
already_paid >= 0);
ALTER TABLE bill ADD CONSTRAINT excessive_payment CHECK (-- 'already_paid' must be less or equal to 'total_cost'
already_paid <= total_cost);

ALTER TABLE specialization ADD UNIQUE (spec_name);
ALTER TABLE specialization ADD CONSTRAINT constraint_0 CHECK (--  'spec_name' field only contains letters and spaces
spec_name~ '^[a-zA-Z ]+$');
ALTER TABLE effect_properties ADD CONSTRAINT effect_properties_fk1 FOREIGN KEY (side_effect_effect_id) REFERENCES side_effect(effect_id);
ALTER TABLE effect_properties ADD CONSTRAINT effect_properties_fk2 FOREIGN KEY (medication_med_id) REFERENCES medication(med_id);
ALTER TABLE effect_properties ADD CONSTRAINT constraint_0 CHECK (-- 'severity' must be positive
severity > 0);
ALTER TABLE effect_properties ADD CONSTRAINT constraint_1 CHECK (-- 'probability' must be between 0 and 1
probability > 0 AND probability < 1);
ALTER TABLE enrolment_appointment ADD CONSTRAINT enrolment_appointment_fk1 FOREIGN KEY (role_role_id) REFERENCES role(role_id);
ALTER TABLE enrolment_appointment ADD CONSTRAINT enrolment_appointment_fk2 FOREIGN KEY (appointment_app_id) REFERENCES appointment(app_id);
ALTER TABLE enrolment_appointment ADD CONSTRAINT enrolment_appointment_fk3 FOREIGN KEY (nurse_employee_contract_service_user_user_id) REFERENCES nurse(employee_contract_service_user_user_id);
ALTER TABLE enrolment_surgery ADD CONSTRAINT enrolment_surgery_fk1 FOREIGN KEY (role_role_id) REFERENCES role(role_id);
ALTER TABLE enrolment_surgery ADD CONSTRAINT enrolment_surgery_fk2 FOREIGN KEY (surgery_surgery_id) REFERENCES surgery(surgery_id);
ALTER TABLE enrolment_surgery ADD CONSTRAINT enrolment_surgery_fk3 FOREIGN KEY (nurse_employee_contract_service_user_user_id) REFERENCES nurse(employee_contract_service_user_user_id);
ALTER TABLE payment ADD UNIQUE (payment_id);
ALTER TABLE payment ADD CONSTRAINT payment_fk1 FOREIGN KEY (bill_bill_id) REFERENCES bill(bill_id);
ALTER TABLE payment ADD CONSTRAINT positive_payment CHECK (-- 'amount' must be positive
amount > 0);
ALTER TABLE dose ADD CONSTRAINT dose_fk1 FOREIGN KEY (medication_med_id) REFERENCES medication(med_id);
ALTER TABLE dose ADD CONSTRAINT dose_fk2 FOREIGN KEY (prescription_presc_id) REFERENCES prescription(presc_id);
ALTER TABLE dose ADD CONSTRAINT constraint_0 CHECK (-- 'amount' must be positive
amount > 0);
ALTER TABLE dose ADD CONSTRAINT time_of_day_constraint CHECK (-- 'time_of_day' has the domain morning, afternoon or evening
(LOWER(time_of_day) IN ('morning', 'afternoon', 'evening')));
ALTER TABLE rank ADD UNIQUE (rank_name);
ALTER TABLE rank ADD CONSTRAINT constraint_0 CHECK (--  'rank_name' field only contains letters and spaces
rank_name ~ '^[a-zA-Z ]+$');
ALTER TABLE role ADD UNIQUE (role_name);
ALTER TABLE role ADD CONSTRAINT constraint_0 CHECK (--  'role_name' field only contains letters and spaces
role_name ~ '^[a-zA-Z ]+$');
ALTER TABLE app_type ADD UNIQUE (type_name);
ALTER TABLE app_type ADD CONSTRAINT constraint_0 CHECK (--  'type_name' field only contains letters and spaces
type_name ~ '^[a-zA-Z ]+$');
ALTER TABLE rank_rank ADD CONSTRAINT rank_rank_fk1 FOREIGN KEY (rank_rank_id) REFERENCES rank(rank_id);
ALTER TABLE rank_rank ADD CONSTRAINT rank_rank_fk2 FOREIGN KEY (rank_rank_id1) REFERENCES rank(rank_id);
ALTER TABLE specialization_specialization ADD CONSTRAINT specialization_specialization_fk1 FOREIGN KEY (specialization_spec_id) REFERENCES specialization(spec_id);
ALTER TABLE specialization_specialization ADD CONSTRAINT specialization_specialization_fk2 FOREIGN KEY (specialization_spec_id1) REFERENCES specialization(spec_id);
ALTER TABLE specialization_doctor ADD CONSTRAINT specialization_doctor_fk1 FOREIGN KEY (specialization_spec_id) REFERENCES specialization(spec_id);
ALTER TABLE specialization_doctor ADD CONSTRAINT specialization_doctor_fk2 FOREIGN KEY (doctor_employee_contract_service_user_user_id) REFERENCES doctor(employee_contract_service_user_user_id);
ALTER TABLE prescription_appointment ADD CONSTRAINT prescription_appointment_fk1 FOREIGN KEY (prescription_presc_id) REFERENCES prescription(presc_id);
ALTER TABLE prescription_appointment ADD CONSTRAINT prescription_appointment_fk2 FOREIGN KEY (appointment_app_id) REFERENCES appointment(app_id);
ALTER TABLE hospitalization_prescription ADD CONSTRAINT hospitalization_prescription_fk1 FOREIGN KEY (hospitalization_hosp_id) REFERENCES hospitalization(hosp_id);
ALTER TABLE hospitalization_prescription ADD CONSTRAINT hospitalization_prescription_fk2 FOREIGN KEY (prescription_presc_id) REFERENCES prescription(presc_id);

CREATE TABLE service_user (
	user_id	 BIGSERIAL,
	name	 VARCHAR(512) NOT NULL,
	nationality VARCHAR(512) NOT NULL,
	phone	 INTEGER NOT NULL,
	birthday	 DATE NOT NULL,
	email	 VARCHAR(512) NOT NULL,
	password	 VARCHAR(512) NOT NULL,
	PRIMARY KEY(user_id)
);

CREATE TABLE employee_contract (
	contract_contract_id BIGSERIAL NOT NULL,
	contract_start_date	 TIMESTAMP NOT NULL,
	contract_end_date	 TIMESTAMP NOT NULL,
	service_user_user_id BIGINT,
	PRIMARY KEY(service_user_user_id)
);

CREATE TABLE assistant (
	employee_contract_service_user_user_id BIGINT,
	PRIMARY KEY(employee_contract_service_user_user_id)
);

CREATE TABLE nurse (
	rank_rank_id				 BIGINT NOT NULL,
	employee_contract_service_user_user_id BIGINT,
	PRIMARY KEY(employee_contract_service_user_user_id)
);

CREATE TABLE doctor (
	licence_id				 BIGSERIAL NOT NULL,
	university				 VARCHAR(512) NOT NULL,
	graduation_date			 DATE NOT NULL,
	employee_contract_service_user_user_id BIGINT,
	PRIMARY KEY(employee_contract_service_user_user_id)
);

CREATE TABLE patient (
	service_user_user_id BIGINT,
	PRIMARY KEY(service_user_user_id)
);

CREATE TABLE appointment (
	app_id					 BIGSERIAL,
	app_date					 TIMESTAMP NOT NULL,
	app_type_type_id				 BIGINT NOT NULL,
	bill_bill_id					 BIGINT NOT NULL,
	patient_service_user_user_id			 BIGINT NOT NULL,
	doctor_employee_contract_service_user_user_id BIGINT NOT NULL,
	PRIMARY KEY(app_id)
);

CREATE TABLE surgery (
	surgery_id					 BIGSERIAL,
	type						 VARCHAR(512) NOT NULL,
	surg_date					 TIMESTAMP NOT NULL,
	hospitalization_hosp_id			 BIGINT NOT NULL,
	doctor_employee_contract_service_user_user_id BIGINT NOT NULL,
	PRIMARY KEY(surgery_id)
);

CREATE TABLE hospitalization (
	hosp_id					 BIGSERIAL,
	start_date					 TIMESTAMP NOT NULL,
	end_date					 TIMESTAMP NOT NULL,
	bill_bill_id				 BIGINT NOT NULL,
	patient_service_user_user_id		 BIGINT NOT NULL,
	nurse_employee_contract_service_user_user_id BIGINT NOT NULL,
	PRIMARY KEY(hosp_id)
);

CREATE TABLE prescription (
	presc_id	 BIGSERIAL,
	validity	DATE NOT NULL,
	PRIMARY KEY(presc_id)
);

CREATE TABLE medication (
	med_id	 BIGSERIAL,
	med_name VARCHAR(512) NOT NULL,
	PRIMARY KEY(med_id)
);

CREATE TABLE side_effect (
	effect_id BIGSERIAL,
	symptom	 VARCHAR(512) NOT NULL,
	PRIMARY KEY(effect_id)
);

CREATE TABLE bill (
	bill_id	 BIGSERIAL,
	total_cost	 BIGINT NOT NULL,
	already_paid	 BIGINT NOT NULL,
	PRIMARY KEY(bill_id)
);

CREATE TABLE specialization (
	spec_id	 BIGSERIAL,
	spec_name VARCHAR(512) NOT NULL,
	PRIMARY KEY(spec_id)
);

CREATE TABLE effect_properties (
	probability		 FLOAT(8) NOT NULL,
	severity		 NUMERIC(8,2) NOT NULL,
	side_effect_effect_id BIGINT,
	medication_med_id	 BIGINT,
	PRIMARY KEY(side_effect_effect_id,medication_med_id)
);

CREATE TABLE enrolment_appointment (
	role_role_id				 BIGINT,
	appointment_app_id				 BIGINT,
	nurse_employee_contract_service_user_user_id BIGINT,
	PRIMARY KEY(role_role_id,appointment_app_id,nurse_employee_contract_service_user_user_id)
);

CREATE TABLE enrolment_surgery (
	role_role_id				 BIGINT,
	surgery_surgery_id				 BIGINT,
	nurse_employee_contract_service_user_user_id BIGINT,
	PRIMARY KEY(role_role_id,surgery_surgery_id,nurse_employee_contract_service_user_user_id)
);

CREATE TABLE payment (
	payment_id	 BIGSERIAL NOT NULL,
	amount	 BIGINT NOT NULL,
	payment_date	TIMESTAMP NOT NULL,
	payment_method VARCHAR(512) NOT NULL,
	bill_bill_id	 BIGINT,
	PRIMARY KEY(payment_id, bill_bill_id)
);

CREATE TABLE dose (
	amount		 INTEGER NOT NULL,
	time_of_day		 VARCHAR(512),
	medication_med_id	 BIGINT,
	prescription_presc_id BIGINT,
	PRIMARY KEY(time_of_day,medication_med_id,prescription_presc_id)
);

CREATE TABLE rank (
	rank_id	 BIGSERIAL,
	rank_name VARCHAR(512) NOT NULL,
	PRIMARY KEY(rank_id)
);

CREATE TABLE role (
	role_id	 BIGSERIAL,
	role_name VARCHAR(512) NOT NULL,
	PRIMARY KEY(role_id)
);

CREATE TABLE app_type (
	type_id	 BIGSERIAL,
	type_name VARCHAR(512) NOT NULL,
	PRIMARY KEY(type_id)
);

CREATE TABLE rank_rank (
	rank_rank_id	 BIGINT,
	rank_rank_id1 BIGINT NOT NULL,
	PRIMARY KEY(rank_rank_id)
);

CREATE TABLE specialization_specialization (
	specialization_spec_id	 BIGINT,
	specialization_spec_id1 BIGINT NOT NULL
);

CREATE TABLE specialization_doctor (
	specialization_spec_id			 BIGINT,
	doctor_employee_contract_service_user_user_id BIGINT,
	PRIMARY KEY(specialization_spec_id,doctor_employee_contract_service_user_user_id)
);

CREATE TABLE prescription_appointment (
	prescription_presc_id BIGINT,
	appointment_app_id	 BIGINT NOT NULL,
	PRIMARY KEY(prescription_presc_id)
);

CREATE TABLE hospitalization_prescription (
	hospitalization_hosp_id BIGINT NOT NULL,
	prescription_presc_id	 BIGINT,
	PRIMARY KEY(prescription_presc_id)
);



-- INDEXES

-- Indexes for date fields
CREATE INDEX idx_employee_contract_start_date
ON employee_contract (contract_start_date);

CREATE INDEX idx_employee_contract_end_date
ON employee_contract (contract_end_date);

CREATE INDEX idx_doctor_graduation_date
ON doctor (graduation_date);

CREATE INDEX idx_appointment_app_date
ON appointment (app_date);

CREATE INDEX idx_surgery_surg_date
ON surgery (surg_date);

CREATE INDEX idx_hospitalization_start_date
ON hospitalization (start_date);

CREATE INDEX idx_hospitalization_end_date
ON hospitalization (end_date);

CREATE INDEX idx_payment_payment_date
ON payment (payment_date);

-- Indexes for user names
CREATE INDEX idx_service_user_name
ON service_user (name);

-- Indexes for user IDs
CREATE INDEX idx_service_user_user_id
ON service_user (user_id);

CREATE INDEX idx_employee_contract_service_user_user_id
ON employee_contract (service_user_user_id);

CREATE INDEX idx_assistant_employee_contract_service_user_user_id
ON assistant (employee_contract_service_user_user_id);

CREATE INDEX idx_nurse_employee_contract_service_user_user_id
ON nurse (employee_contract_service_user_user_id);

CREATE INDEX idx_doctor_employee_contract_service_user_user_id
ON doctor (employee_contract_service_user_user_id);

CREATE INDEX idx_patient_service_user_user_id
ON patient (service_user_user_id);

-- Indexes for emails
CREATE INDEX idx_service_user_email
ON service_user (email);

-- Indexes for bill IDs
CREATE INDEX idx_bill_bill_id
ON bill (bill_id);


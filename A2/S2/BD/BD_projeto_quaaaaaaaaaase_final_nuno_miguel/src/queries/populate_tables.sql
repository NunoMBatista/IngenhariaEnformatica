-- NURSE RANKS TABLE
-- Insert ranks into the rank table

INSERT INTO rank (rank_id, rank_name) VALUES (1, 'Junior Nurse');
INSERT INTO rank (rank_id, rank_name) VALUES (2, 'Senior Nurse');
INSERT INTO rank (rank_id, rank_name) VALUES (3, 'Head Nurse');

-- Establish hierarchy in the rank_rank table

INSERT INTO rank_rank (rank_rank_id, rank_rank_id1) VALUES (1, 2); -- Junior Nurse is below Senior Nurse
INSERT INTO rank_rank (rank_rank_id, rank_rank_id1) VALUES (2, 3); -- Senior Nurse is below Head Nurse


-- DOCTOR SPECIALIZATIONS TABLE
-- Insert specializations into the specialization table

-- Neurology
INSERT INTO specialization (spec_id, spec_name) VALUES (1, 'Neurology');
INSERT INTO specialization (spec_id, spec_name) VALUES (2, 'Pediatric Neurology');
INSERT INTO specialization (spec_id, spec_name) VALUES (3, 'Neurophysiology');
INSERT INTO specialization (spec_id, spec_name) VALUES (4, 'Neuropathology');

-- Radiology
INSERT INTO specialization (spec_id, spec_name) VALUES (5, 'Radiology');
INSERT INTO specialization (spec_id, spec_name) VALUES (6, 'Thoracic Radiology');
INSERT INTO specialization (spec_id, spec_name) VALUES (7, 'Interventional Radiology');
INSERT INTO specialization (spec_id, spec_name) VALUES (8, 'Head and Neck Radiology');

-- Cardiology
INSERT INTO specialization (spec_id, spec_name) VALUES (9, 'Cardiology');
INSERT INTO specialization (spec_id, spec_name) VALUES (10, 'Pediatric Cardiology');
INSERT INTO specialization (spec_id, spec_name) VALUES (11, 'Interventional Cardiology');
INSERT INTO specialization (spec_id, spec_name) VALUES (12, 'Cardiac Electrophysiology');


-- Establish hierarchy in the specialization_specialization table   

-- Neurology hierarchy tree
INSERT INTO specialization_specialization (specialization_spec_id, specialization_spec_id1) VALUES (1, 2); -- Pediatric Neurology is a sub-specialization of Neurology
INSERT INTO specialization_specialization (specialization_spec_id, specialization_spec_id1) VALUES (1, 3); -- Neurophysiology is a sub-specialization of Neurology
INSERT INTO specialization_specialization (specialization_spec_id, specialization_spec_id1) VALUES (1, 4); -- Neuroopathology is a sub-specialization of Neurology

-- Radiology hierarchy tree
INSERT INTO specialization_specialization (specialization_spec_id, specialization_spec_id1) VALUES (5, 6); -- Thoracic Radiology is a sub-specialization of Radiology
INSERT INTO specialization_specialization (specialization_spec_id, specialization_spec_id1) VALUES (5, 7); -- Interventional Radiology is a sub-specialization of Radiology
INSERT INTO specialization_specialization (specialization_spec_id, specialization_spec_id1) VALUES (5, 8); -- Head and Neck Radiology is a sub-specialization of Radiology

-- Cardiology hierarchy tree
INSERT INTO specialization_specialization (specialization_spec_id, specialization_spec_id1) VALUES (9, 10); -- Pediatric Cardiology is a sub-specialization of Cardiology   
INSERT INTO specialization_specialization (specialization_spec_id, specialization_spec_id1) VALUES (9, 11); -- Interventional Cardiology is a sub-specialization of Cardiology
INSERT INTO specialization_specialization (specialization_spec_id, specialization_spec_id1) VALUES (9, 12); -- Cardiac Electrophysiology is a sub-specialization of Cardiology




-- Nurse roles table
-- Insert roles into the role table
INSERT INTO role (role_id, role_name) VALUES (1, 'Euthanasist');
INSERT INTO role (role_id, role_name) VALUES (2, 'Instrument');
INSERT INTO role (role_id, role_name) VALUES (3, 'Patient Advocacy');
INSERT INTO role (role_id, role_name) VALUES (4, 'Preoperative');


-- Appointment types table
-- Insert appointment types into the appointment_type table
INSERT INTO app_type (type_id, type_name) VALUES (1, 'Primary Care');
INSERT INTO app_type (type_id, type_name) VALUES (2, 'Psychotherapy');
INSERT INTO app_type (type_id, type_name) VALUES (3, 'Specialist Consultation');
INSERT INTO app_type (type_id, type_name) VALUES (4, 'Physiotherapy');




-- Medication table
-- Insert medications into the medication table
INSERT INTO medication (med_id, med_name) VALUES (1, 'Paracetamol');
INSERT INTO medication (med_id, med_name) VALUES (2, 'Ibuprofen');
INSERT INTO medication (med_id, med_name) VALUES (3, 'Aspirin');
INSERT INTO medication (med_id, med_name) VALUES (4, 'Morphine');
INSERT INTO medication (med_id, med_name) VALUES (5, 'Codeine');
INSERT INTO medication (med_id, med_name) VALUES (6, 'Quetiapine');

-- Side effect table
-- Insert side effects into the side effect table
INSERT INTO side_effect (effect_id, symptom) VALUES (1, 'Nausea');
INSERT INTO side_effect (effect_id, symptom) VALUES (2, 'Vomiting');
INSERT INTO side_effect (effect_id, symptom) VALUES (3, 'Diarrhea');
INSERT INTO side_effect (effect_id, symptom) VALUES (4, 'Constipation');
INSERT INTO side_effect (effect_id, symptom) VALUES (5, 'Dizziness');
INSERT INTO side_effect (effect_id, symptom) VALUES (6, 'Headache');
INSERT INTO side_effect (effect_id, symptom) VALUES (7, 'Insomnia');
INSERT INTO side_effect (effect_id, symptom) VALUES (8, 'Weight gain');
INSERT INTO side_effect (effect_id, symptom) VALUES (9, 'Weight loss');
INSERT INTO side_effect (effect_id, symptom) VALUES (10, 'Dry mouth');
INSERT INTO side_effect (effect_id, symptom) VALUES (11, 'Blurred vision');
INSERT INTO side_effect (effect_id, symptom) VALUES (12, 'Tremors');

-- Effect properties table
-- Insert effect properties into the effect properties table
INSERT INTO effect_properties (probability, severity, side_effect_effect_id, medication_med_id) VALUES (0.1, 4, 1, 1);
INSERT INTO effect_properties (probability, severity, side_effect_effect_id, medication_med_id) VALUES (0.3, 6, 3, 1);

INSERT INTO effect_properties (probability, severity, side_effect_effect_id, medication_med_id) VALUES (0.2, 8, 10, 2);
INSERT INTO effect_properties (probability, severity, side_effect_effect_id, medication_med_id) VALUES (0.3, 6, 6, 2);

INSERT INTO effect_properties (probability, severity, side_effect_effect_id, medication_med_id) VALUES (0.2, 2, 2, 3);
INSERT INTO effect_properties (probability, severity, side_effect_effect_id, medication_med_id) VALUES (0.3, 6, 3, 3);

INSERT INTO effect_properties (probability, severity, side_effect_effect_id, medication_med_id) VALUES (0.1, 4, 1, 4);
INSERT INTO effect_properties (probability, severity, side_effect_effect_id, medication_med_id) VALUES (0.2, 2, 2, 4);

INSERT INTO effect_properties (probability, severity, side_effect_effect_id, medication_med_id) VALUES (0.1, 4, 1, 5);
INSERT INTO effect_properties (probability, severity, side_effect_effect_id, medication_med_id) VALUES (0.3, 6, 3, 5);

INSERT INTO effect_properties (probability, severity, side_effect_effect_id, medication_med_id) VALUES (0.2, 8, 10, 6);
INSERT INTO effect_properties (probability, severity, side_effect_effect_id, medication_med_id) VALUES (0.3, 6, 6, 6);



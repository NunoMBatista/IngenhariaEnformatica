BEGIN;

SELECT 
    p.presc_id AS prescription_id, 
    p.validity,
    json_agg(json_build_object(
                            'amount', d.amount, 
                            'medicine', m.med_name, 
                            'posology_frequency', d.time_of_day)) AS posology
FROM prescription p
JOIN dose d ON p.presc_id = d.prescription_presc_id -- JOIN prescription's doses
JOIN medication m ON d.medication_med_id = m.med_id -- JOIN dose's medication

-- JOIN prescription's appointment
LEFT JOIN prescription_appointment pa ON p.presc_id = pa.prescription_presc_id
LEFT JOIN appointment a ON pa.appointment_app_id = a.app_id 

-- JOIN prescription's hospitalization
LEFT JOIN hospitalization_prescription hp ON p.presc_id = hp.prescription_presc_id 
LEFT JOIN hospitalization h ON hp.hospitalization_hosp_id = h.hosp_id

WHERE a.patient_service_user_user_id = %s OR h.patient_service_user_user_id = %s
GROUP BY p.presc_id, p.validity

COMMIT;
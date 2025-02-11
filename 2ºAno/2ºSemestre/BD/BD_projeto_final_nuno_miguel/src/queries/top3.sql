BEGIN;
SET TRANSACTION ISOLATION LEVEL SERIALIZABLE; -- Set the transaction isolation level to SERIALIZABLE to prevent dirty reads

WITH top_patients AS (
    -- Get the sum of payments for each patient
    SELECT patient_service_user_user_id AS patient_id, SUM(amount) AS total_paid
    FROM payment 
    -- We need to join the payment with the bill to get the patient ID from the associated service
    JOIN bill ON payment.bill_bill_id = bill.bill_id 
    JOIN (
        SELECT bill_bill_id, patient_service_user_user_id FROM appointment -- Get the patient ID from the appointment associated with the bill
        UNION ALL
        SELECT bill_bill_id, patient_service_user_user_id FROM hospitalization -- Get the patient ID from the hospitalization associated with the bill
    ) AS bill_user ON bill.bill_id = bill_user.bill_bill_id
    WHERE payment_date >= NOW() - INTERVAL '30 days' -- Filter payments from the last 30 days
    GROUP BY patient_service_user_user_id -- Aggregate by patient ID
    ORDER BY total_paid DESC 
    LIMIT 3 -- Only get the top 3 patients 
)

SELECT su.name, tp.total_paid, 
    -- Build an array of procedures for each patient
       array_agg(json_build_object('procedure_id', COALESCE(a.app_id, h.hosp_id), 
                                   'doctor_id', COALESCE(a.doctor_employee_contract_service_user_user_id, h.nurse_employee_contract_service_user_user_id), 
                                   'date', COALESCE(a.app_date, h.start_date))) AS procedures

FROM top_patients tp
JOIN patient p ON tp.patient_id = p.service_user_user_id -- Join the patient with the top patients
JOIN service_user su ON p.service_user_user_id = su.user_id -- Join the service user with the patient
LEFT JOIN appointment a ON tp.patient_id = a.patient_service_user_user_id -- Join the appointment with the top patients
LEFT JOIN hospitalization h ON tp.patient_id = h.patient_service_user_user_id -- Join the hospitalization with the top patients
GROUP BY su.name, tp.total_paid -- Group by patient name and total paid
ORDER BY tp.total_paid DESC; -- Order by total paid in descending order


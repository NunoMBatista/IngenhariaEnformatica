BEGIN;
SET TRANSACTION ISOLATION LEVEL SERIALIZABLE; -- Set the transaction isolation level to SERIALIZABLE to prevent dirty reads

WITH months AS ( -- We need to generate a list of months to ensure we have a row for each month, even if there are no surgeries
    SELECT TO_CHAR(date_trunc(
                    'month', 
                    -- Generate a series of dates from 1 year ago to now, with a step of 1 month and format it as 'YYYY Month'
                    generate_series(NOW() - INTERVAL '11 months', NOW(), '1 month')), 'YYYY Month')    
                    AS month
    ),
surgeries AS (
    SELECT
    TO_CHAR(date_trunc('month', surgery.surg_date), 'YYYY Month') AS month,
    service_user.name,
    COUNT(*) AS total_surgeries,
    -- Rank the doctors by the number of surgeries they performed in each month
    RANK() OVER (PARTITION BY TO_CHAR(date_trunc('month', surgery.surg_date), 'YYYY Month') ORDER BY COUNT(*) DESC) as rank
    FROM surgery
    JOIN service_user ON service_user.user_id = surgery.doctor_employee_contract_service_user_user_id
    WHERE surgery.surg_date >= (NOW() - INTERVAL '1 year')
    GROUP BY TO_CHAR(date_trunc('month', surgery.surg_date), 'YYYY Month'), service_user.name
    )

SELECT 
    months.month, 
    COALESCE(surgeries.name) AS name, -- If there are no surgeries, just don't show the doctor name
    COALESCE(surgeries.total_surgeries, 0) AS total_surgeries 
FROM months
LEFT JOIN surgeries ON months.month = surgeries.month AND surgeries.rank = 1 -- Only show the doctor with the most surgeries in each month
ORDER BY TO_DATE(months.month, 'YYYY Month') DESC;

COMMIT;
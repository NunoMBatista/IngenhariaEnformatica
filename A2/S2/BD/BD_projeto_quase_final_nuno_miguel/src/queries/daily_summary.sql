BEGIN;
SET TRANSACTION ISOLATION LEVEL SERIALIZABLE; -- Set the transaction isolation level to SERIALIZABLE to prevent dirty reads

WITH payments_sum AS (
    SELECT SUM(amount) AS amount_spent -- Get the sum of every payment 
    FROM payment
    WHERE TO_DATE(payment_date::text, 'YYYY-MM-DD') = TO_DATE(%s, 'YYYY-MM-DD') -- Only get the payments from the given date
    ),
    surgeries_count AS ( 
        SELECT COUNT(*) AS surgeries -- Count number of surgery entries 
        FROM surgery
        WHERE TO_DATE(surg_date::text, 'YYYY-MM-DD') = TO_DATE(%s, 'YYYY-MM-DD') -- Only get the surgeries from the given date
    ),
    prescriptions_count AS (
        SELECT COUNT(*) AS prescriptions -- Count number of prescription entries
        FROM prescription
        WHERE validity = TO_DATE(%s, 'YYYY-MM-DD') -- Only get the prescriptions from the given date
    )
SELECT
    -- Coalesce the values to 0 if they are NULL, as we want to show 0 if there are no entries
    COALESCE((SELECT amount_spent FROM payments_sum), 0) AS amount_spent,
    COALESCE((SELECT surgeries FROM surgeries_count), 0) AS surgeries,
    COALESCE((SELECT prescriptions FROM prescriptions_count), 0) AS prescriptions; 

COMMIT;
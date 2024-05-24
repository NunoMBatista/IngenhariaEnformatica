INSERT INTO employee_contract (service_user_user_id, contract_start_date, contract_end_date)
VALUES (%s, %s, %s)
RETURNING contract_contract_id;

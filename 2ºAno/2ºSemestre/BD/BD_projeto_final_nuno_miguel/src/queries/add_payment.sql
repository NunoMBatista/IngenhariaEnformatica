INSERT INTO payment (payment_id, amount, payment_date,payment_method bill_bill_id)
VALUES (%s, %s, %s, %s,%s)
RETURNING payment_id;
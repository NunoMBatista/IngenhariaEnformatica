INSERT INTO service_user (name, nationality, phone, birthday, email, password)
VALUES (%s, %s, %s, %s, %s, %s)
RETURNING user_id;
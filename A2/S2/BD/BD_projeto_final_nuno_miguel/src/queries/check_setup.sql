SELECT 'service_user' AS table_name, EXISTS (
   SELECT FROM information_schema.tables 
   WHERE  table_schema = 'public'
   AND    table_name   = 'service_user'
   ) AS exists
UNION ALL
SELECT 'employee_contract', EXISTS (
   SELECT FROM information_schema.tables 
   WHERE  table_schema = 'public'
   AND    table_name   = 'employee_contract'
   )
UNION ALL
SELECT 'assistant', EXISTS (
   SELECT FROM information_schema.tables 
   WHERE  table_schema = 'public'
   AND    table_name   = 'assistant'
   )
UNION ALL
SELECT 'nurse', EXISTS (
   SELECT FROM information_schema.tables 
   WHERE  table_schema = 'public'
   AND    table_name   = 'nurse'
   )
UNION ALL
SELECT 'doctor', EXISTS (
   SELECT FROM information_schema.tables 
   WHERE  table_schema = 'public'
   AND    table_name   = 'doctor'
   )
UNION ALL
SELECT 'patient', EXISTS (
   SELECT FROM information_schema.tables 
   WHERE  table_schema = 'public'
   AND    table_name   = 'patient'
   )
UNION ALL
SELECT 'appointment', EXISTS (
   SELECT FROM information_schema.tables 
   WHERE  table_schema = 'public'
   AND    table_name   = 'appointment'
   )
UNION ALL
SELECT 'surgery', EXISTS (
   SELECT FROM information_schema.tables 
   WHERE  table_schema = 'public'
   AND    table_name   = 'surgery'
   )
UNION ALL
SELECT 'hospitalization', EXISTS (
   SELECT FROM information_schema.tables 
   WHERE  table_schema = 'public'
   AND    table_name   = 'hospitalization'
   )
UNION ALL
SELECT 'prescription', EXISTS (
   SELECT FROM information_schema.tables 
   WHERE  table_schema = 'public'
   AND    table_name   = 'prescription'
   )
UNION ALL
SELECT 'medication', EXISTS (
   SELECT FROM information_schema.tables 
   WHERE  table_schema = 'public'
   AND    table_name   = 'medication'
   )
UNION ALL
SELECT 'side_effect', EXISTS (
   SELECT FROM information_schema.tables 
   WHERE  table_schema = 'public'
   AND    table_name   = 'side_effect'
   )
UNION ALL
SELECT 'bill', EXISTS (
   SELECT FROM information_schema.tables 
   WHERE  table_schema = 'public'
   AND    table_name   = 'bill'
   )
UNION ALL
SELECT 'specialization', EXISTS (
   SELECT FROM information_schema.tables 
   WHERE  table_schema = 'public'
   AND    table_name   = 'specialization'
   )
UNION ALL
SELECT 'effect_properties', EXISTS (
   SELECT FROM information_schema.tables 
   WHERE  table_schema = 'public'
   AND    table_name   = 'effect_properties'
   )
UNION ALL
SELECT 'enrolment_appointment', EXISTS (
   SELECT FROM information_schema.tables 
   WHERE  table_schema = 'public'
   AND    table_name   = 'enrolment_appointment'
   )
UNION ALL
SELECT 'enrolment_surgery', EXISTS (
   SELECT FROM information_schema.tables 
   WHERE  table_schema = 'public'
   AND    table_name   = 'enrolment_surgery'
   )
UNION ALL
SELECT 'payment', EXISTS (
   SELECT FROM information_schema.tables 
   WHERE  table_schema = 'public'
   AND    table_name   = 'payment'
   )
UNION ALL
SELECT 'dose', EXISTS (
   SELECT FROM information_schema.tables 
   WHERE  table_schema = 'public'
   AND    table_name   = 'dose'
   )
UNION ALL
SELECT 'rank', EXISTS (
   SELECT FROM information_schema.tables 
   WHERE  table_schema = 'public'
   AND    table_name   = 'rank'
   )
UNION ALL
SELECT 'role', EXISTS (
   SELECT FROM information_schema.tables 
   WHERE  table_schema = 'public'
   AND    table_name   = 'role'
   )
UNION ALL
SELECT 'app_type', EXISTS (
   SELECT FROM information_schema.tables 
   WHERE  table_schema = 'public'
   AND    table_name   = 'app_type'
   )
UNION ALL
SELECT 'rank_rank', EXISTS (
   SELECT FROM information_schema.tables 
   WHERE  table_schema = 'public'
   AND    table_name   = 'rank_rank'
   )
UNION ALL
SELECT 'specialization_specialization', EXISTS (
   SELECT FROM information_schema.tables 
   WHERE  table_schema = 'public'
   AND    table_name   = 'specialization_specialization'
   )
UNION ALL
SELECT 'specialization_doctor', EXISTS (
   SELECT FROM information_schema.tables 
   WHERE  table_schema = 'public'
   AND    table_name   = 'specialization_doctor'
   )
UNION ALL
SELECT 'prescription_appointment', EXISTS (
   SELECT FROM information_schema.tables 
   WHERE  table_schema = 'public'
   AND    table_name   = 'prescription_appointment'
   )
UNION ALL
SELECT 'hospitalization_prescription', EXISTS (
   SELECT FROM information_schema.tables 
   WHERE  table_schema = 'public'
   AND    table_name   = 'hospitalization_prescription'
   );
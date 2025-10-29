REVOKE CONNECT ON DATABASE hms_db FROM PUBLIC; -- Remove the connection previleges from everyone
SELECT pg_terminate_backend(pg_stat_activity.pid) -- Select the pid of the processes connected to the DB

FROM pg_stat_activity
WHERE pg_stat_activity.datname = 'hms_db'
AND pid <> pg_backend_pid();
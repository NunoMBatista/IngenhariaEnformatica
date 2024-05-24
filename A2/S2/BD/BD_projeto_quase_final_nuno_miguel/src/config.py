from datetime import timedelta

class Config:
    JWT_SECRET_KEY = '1234'
    JWT_TOKEN_LOCATION = ['cookies', 'headers']
    JWT_ACCESS_TOKEN_EXPIRES = timedelta(minutes=30)
    WTF_CSRF_ENABLED = False
    JWT_COOKIE_CSRF_PROTECT = False
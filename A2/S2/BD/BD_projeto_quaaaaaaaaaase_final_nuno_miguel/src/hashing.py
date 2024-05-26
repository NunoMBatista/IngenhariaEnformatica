import hashlib
import os

ITERATIONS = 100000
ALGORITHM = 'sha256'

def hash_password(password):
    # Generate salt
    salt = os.urandom(32)
    
    key = hashlib.pbkdf2_hmac(
        ALGORITHM, # The hash digest algorithm for HMAC
        password.encode('utf-8'), # Convert the password to bytes
        salt, # Provide the salt
        ITERATIONS # Use 1e6 iterations
    )
    
    return salt + key

def verify_password(stored_key, provided_password):
    # Convert the stored key to bytes
    stored_key = bytes.fromhex(stored_key.replace("\\x", "")) # Convert the stored key to bytes
    # Get the salt from the stored password
    salt = stored_key[:32]
    # Get the key from the stored password
    stored_key = stored_key[32:]
    # Hash the provided password
    new_key = hashlib.pbkdf2_hmac(
        ALGORITHM,
        provided_password.encode('utf-8'),
        salt,
        ITERATIONS
    )

    # Compare the stored key with the new key
    return new_key == stored_key
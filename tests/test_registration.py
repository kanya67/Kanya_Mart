import urllib.request
import json
import sys

BASE_URL = "http://localhost:8080/api/v1/auth/register"

def post_json(data):
    req = urllib.request.Request(BASE_URL, data=json.dumps(data).encode('utf-8'),
                                 headers={'Content-Type': 'application/json'})
    try:
        response = urllib.request.urlopen(req)
        return response.getcode(), json.loads(response.read().decode('utf-8'))
    except urllib.error.HTTPError as e:
        return e.code, json.loads(e.read().decode('utf-8'))
    except Exception as e:
        print(f"Error connecting: {e}")
        return 0, {}

def run_tests():
    print("Running Registration API Tests...")
    
    # Test 1: Successful Registration
    status, res = post_json({"username": "testuser", "email": "test@example.com", "password": "password123"})
    assert status == 201, f"Expected 201, got {status}"
    assert "password" not in res and "password_hash" not in res, "Password leaked in response!"
    assert "id" in res, "Missing UUID"
    print("[PASS] Successful registration")

    # Test 2: Duplicate Username
    status, res = post_json({"username": "testuser", "email": "test2@example.com", "password": "password123"})
    assert status == 400, f"Expected 400, got {status}"
    assert res.get("error") == "Username already exists", "Wrong error message"
    print("[PASS] Duplicate username")

    # Test 3: Duplicate Email
    status, res = post_json({"username": "testuser2", "email": "test@example.com", "password": "password123"})
    assert status == 400, f"Expected 400, got {status}"
    assert res.get("error") == "Email already registered", "Wrong error message"
    print("[PASS] Duplicate email")

    # Test 4: Empty fields
    status, res = post_json({"username": "", "email": "test3@example.com", "password": "password123"})
    assert status == 400, f"Expected 400, got {status}"
    print("[PASS] Empty fields validation")

    print("All tests passed successfully!")

if __name__ == "__main__":
    run_tests()

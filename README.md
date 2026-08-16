# KanyaMart

This KanyaMart program is a simple C++ online shopping system. It allows users to log in, view products, search for products, and log out.

## Existing Features
- Product structure: Stores product details like ID, name, category, and price.
- Product list: Contains sample products such as kurtis, sarees, makeup kits, laptops, and mobiles.
- Display Products: Shows all available products.
- Search Products: Allows the user to search using a product name or category.
- Menu-driven console interface.

---

# Registration Flow

We have introduced a complete user registration workflow alongside the console application.

## Architecture
- **Web Server**: An HTTP server runs on a background thread (`httplib`).
- **Frontend**: A sleek, dark-themed HTML/JS UI is served at `http://localhost:8080/login.html`.
- **API Endpoints**: Handled by `AuthController`.
- **Business Logic**: Handled by `AuthService` (validation, duplicate checks, UUID generation, password hashing).
- **Storage**: In-memory `unordered_map<string, User>` protected by a `std::mutex` for thread safety. (Note: Data is lost on server restart).
- **Security**: Passwords are hashed using **bcrypt**. No plaintext passwords are stored or returned. The XOR demo has been removed entirely.
- **Login Integration**: The console login prompt now uses bcrypt to verify passwords against the registered users database.

## Registration API Documentation

### POST `/api/v1/auth/register`

**Request Headers:**
`Content-Type: application/json`

**Request Body:**
```json
{
    "username": "srini",
    "email": "srini@email.com",
    "password": "secret123"
}
```

**Success Response (201 Created):**
```json
{
    "id": "d0a48717-f629-437e-946f-0df5f143c2fb",
    "username": "srini",
    "email": "srini@email.com",
    "role": "customer",
    "created_at": "2026-08-16 12:00:00",
    "updated_at": "2026-08-16 12:00:00"
}
```
*(Notice that `password` and `password_hash` are deliberately excluded).*

**Error Responses (400 Bad Request):**
- Missing fields: `{"error": "Username, email, and password are required"}`
- Duplicate username: `{"error": "Username already exists"}`
- Duplicate email: `{"error": "Email already registered"}`

## Build & Setup Instructions

### Dependencies
This project automatically fetches its dependencies via CMake:
- `yhirose/cpp-httplib` (HTTP server)
- `nlohmann/json` (JSON parser)
- `hilch/Bcrypt.cpp` (bcrypt hashing)
- Standard C++17 threading and mutex capabilities.

### Build Commands
```bash
# 1. Configure the build directory
cmake -S . -B build

# 2. Build the executable
cmake --build build

# 3. Run the application
./build/kanyamart
```

## Testing Instructions
Once the server is running, you can run the automated test suite from another terminal:
```bash
python3 tests/test_registration.py
```
This tests successful registration, duplicate validation, and ensures passwords do not leak.

## Known Limitations
- Users are currently stored in memory. A restart will wipe registered users.
- The console menu continues to run on the main thread and uses `cin`. Do not type while testing the web server if you intend to interact with the console cleanly later.

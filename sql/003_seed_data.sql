-- KanyaMart Seed Data
-- Demo seller and sample products
-- Password for seller1: seller123 (bcrypt hash generated at runtime)
-- Password for admin: admin123

-- NOTE: Passwords must be hashed by the application.
-- This seed file inserts pre-hashed passwords.
-- bcrypt hash of 'seller123' with cost 12:
-- $2a$12$LJ3m4ys2kVqrBzQGHKbGxOqGjGZCnI6TmvGcIqGECO5R7DqR0W6Te
-- bcrypt hash of 'admin123' with cost 12:
-- $2a$12$92IXUNpkjO0rOQ5byMi.Ye4oKoEa3Ro9llC/.og/at2.uheWG/igi

-- Demo seller account
INSERT INTO users (id, username, email, password_hash, role, created_at, updated_at)
VALUES (
    'a0000000-0000-0000-0000-000000000001',
    'seller1',
    'seller1@kanyamart.com',
    '$2a$12$LJ3m4ys2kVqrBzQGHKbGxOqGjGZCnI6TmvGcIqGECO5R7DqR0W6Te',
    'seller',
    NOW(),
    NOW()
) ON CONFLICT (username) DO NOTHING;

-- Demo admin account
INSERT INTO users (id, username, email, password_hash, role, created_at, updated_at)
VALUES (
    'a0000000-0000-0000-0000-000000000002',
    'admin',
    'admin@kanyamart.com',
    '$2a$12$92IXUNpkjO0rOQ5byMi.Ye4oKoEa3Ro9llC/.og/at2.uheWG/igi',
    'admin',
    NOW(),
    NOW()
) ON CONFLICT (username) DO NOTHING;

-- Sample Products (seller1)
INSERT INTO products (id, seller_id, name, description, category, price, stock, image_url, is_active)
VALUES
    -- Electronics
    ('b0000000-0000-0000-0000-000000000001', 'a0000000-0000-0000-0000-000000000001',
     'Quantum Pro Laptop', 'Ultra-thin 15.6" laptop with 32GB RAM, 1TB SSD, RTX 4090 graphics. Perfect for creators and gamers.', 
     'Electronics', 89999.00, 15, '', true),
    
    ('b0000000-0000-0000-0000-000000000002', 'a0000000-0000-0000-0000-000000000001',
     'NexGen Smartphone X1', 'Flagship smartphone with 6.8" AMOLED display, 200MP camera, 5000mAh battery, 5G enabled.',
     'Electronics', 54999.00, 25, '', true),
    
    ('b0000000-0000-0000-0000-000000000003', 'a0000000-0000-0000-0000-000000000001',
     'AuraSound Wireless Earbuds', 'Premium true wireless earbuds with ANC, 40-hour battery life, spatial audio, IPX5 waterproof.',
     'Electronics', 4999.00, 50, '', true),

    -- Fashion
    ('b0000000-0000-0000-0000-000000000004', 'a0000000-0000-0000-0000-000000000001',
     'Royal Silk Saree', 'Handcrafted Banarasi silk saree with intricate zari work. Comes with matching blouse piece.',
     'Fashion', 8999.00, 20, '', true),

    ('b0000000-0000-0000-0000-000000000005', 'a0000000-0000-0000-0000-000000000001',
     'Designer Floral Kurti Set', 'Premium cotton kurti with palazzo pants. Beautiful floral print with hand embroidery.',
     'Fashion', 2499.00, 35, '', true),

    ('b0000000-0000-0000-0000-000000000006', 'a0000000-0000-0000-0000-000000000001',
     'Urban Street Jacket', 'Sleek bomber jacket with reflective strips. Water-resistant fabric, perfect for all seasons.',
     'Fashion', 3999.00, 18, '', true),

    -- Home
    ('b0000000-0000-0000-0000-000000000007', 'a0000000-0000-0000-0000-000000000001',
     'Smart Home Hub Pro', 'Voice-controlled smart home hub. Controls lights, thermostat, security cameras, and more.',
     'Home', 7999.00, 30, '', true),

    ('b0000000-0000-0000-0000-000000000008', 'a0000000-0000-0000-0000-000000000001',
     'Artisan Ceramic Vase Set', 'Set of 3 handmade ceramic vases in graduated sizes. Modern minimalist design.',
     'Home', 2999.00, 40, '', true),

    -- Beauty
    ('b0000000-0000-0000-0000-000000000009', 'a0000000-0000-0000-0000-000000000001',
     'Luxe Glow Makeup Kit', 'Complete 24-piece professional makeup kit. Includes palettes, brushes, primers, and setting spray.',
     'Beauty', 5999.00, 22, '', true),

    ('b0000000-0000-0000-0000-000000000010', 'a0000000-0000-0000-0000-000000000001',
     'Organic Skincare Bundle', 'Natural skincare set with cleanser, toner, serum, moisturizer, and SPF 50 sunscreen.',
     'Beauty', 3499.00, 45, '', true),

    -- Sports
    ('b0000000-0000-0000-0000-000000000011', 'a0000000-0000-0000-0000-000000000001',
     'ProFit Smart Watch Elite', 'Advanced fitness tracker with ECG, SpO2, GPS, swim-proof design. 14-day battery.',
     'Sports', 12999.00, 28, '', true),

    ('b0000000-0000-0000-0000-000000000012', 'a0000000-0000-0000-0000-000000000001',
     'Carbon Fiber Yoga Mat', 'Premium 6mm yoga mat with alignment markers. Non-slip surface, eco-friendly material.',
     'Sports', 1999.00, 55, '', true),

    -- Accessories
    ('b0000000-0000-0000-0000-000000000013', 'a0000000-0000-0000-0000-000000000001',
     'Titan Leather Backpack', 'Full-grain leather backpack with laptop compartment, USB charging port, anti-theft design.',
     'Accessories', 6999.00, 20, '', true),

    ('b0000000-0000-0000-0000-000000000014', 'a0000000-0000-0000-0000-000000000001',
     'Crystal Chronograph Watch', 'Luxury analog watch with sapphire crystal, Swiss movement, stainless steel band.',
     'Accessories', 15999.00, 12, '', true),

    -- Books
    ('b0000000-0000-0000-0000-000000000015', 'a0000000-0000-0000-0000-000000000001',
     'The Art of Code', 'Bestselling book on software engineering principles. 500 pages of insights from industry leaders.',
     'Books', 899.00, 100, '', true),

    ('b0000000-0000-0000-0000-000000000016', 'a0000000-0000-0000-0000-000000000001',
     'Digital Minds Collection', 'Box set of 5 books covering AI, blockchain, quantum computing, IoT, and cybersecurity.',
     'Books', 2499.00, 60, '', true),

    -- Gaming
    ('b0000000-0000-0000-0000-000000000017', 'a0000000-0000-0000-0000-000000000001',
     'HyperX Mechanical Keyboard', 'RGB mechanical gaming keyboard with Cherry MX Red switches, aluminum frame, macro keys.',
     'Gaming', 8499.00, 30, '', true),

    ('b0000000-0000-0000-0000-000000000018', 'a0000000-0000-0000-0000-000000000001',
     'VR Headset Horizon', 'Standalone VR headset with 4K resolution per eye, 120Hz refresh rate, hand tracking.',
     'Gaming', 34999.00, 10, '', true)

ON CONFLICT DO NOTHING;

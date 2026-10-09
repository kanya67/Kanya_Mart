/* ============================================================
   KANYAMART THREE.JS 3D SCENE
   Futuristic particle background for landing page
   ============================================================ */

const ThreeScene = {
    scene: null, camera: null, renderer: null,
    particles: null, rings: [], mouse: { x: 0, y: 0 },
    animationId: null, active: false,

    init() {
        const canvas = document.getElementById('three-canvas');
        if (!canvas || !window.THREE) return;

        // Check reduced motion preference
        if (window.matchMedia('(prefers-reduced-motion: reduce)').matches) return;

        this.scene = new THREE.Scene();
        this.camera = new THREE.PerspectiveCamera(75, window.innerWidth / window.innerHeight, 0.1, 1000);
        this.camera.position.z = 30;

        this.renderer = new THREE.WebGLRenderer({ canvas, alpha: true, antialias: true });
        this.renderer.setSize(window.innerWidth, window.innerHeight);
        this.renderer.setPixelRatio(Math.min(window.devicePixelRatio, 2));

        // Particles
        const particleCount = window.innerWidth < 768 ? 500 : 1500;
        const geometry = new THREE.BufferGeometry();
        const positions = new Float32Array(particleCount * 3);
        const colors = new Float32Array(particleCount * 3);

        for (let i = 0; i < particleCount; i++) {
            positions[i * 3] = (Math.random() - 0.5) * 80;
            positions[i * 3 + 1] = (Math.random() - 0.5) * 80;
            positions[i * 3 + 2] = (Math.random() - 0.5) * 80;

            // Blue to violet gradient
            const t = Math.random();
            colors[i * 3] = t * 0.55;      // R
            colors[i * 3 + 1] = 0.3 + t * 0.5; // G
            colors[i * 3 + 2] = 0.8 + t * 0.2; // B
        }

        geometry.setAttribute('position', new THREE.BufferAttribute(positions, 3));
        geometry.setAttribute('color', new THREE.BufferAttribute(colors, 3));

        const material = new THREE.PointsMaterial({
            size: 0.15,
            vertexColors: true,
            transparent: true,
            opacity: 0.6,
            blending: THREE.AdditiveBlending,
            sizeAttenuation: true
        });

        this.particles = new THREE.Points(geometry, material);
        this.scene.add(this.particles);

        // Glowing rings
        for (let i = 0; i < 3; i++) {
            const ringGeom = new THREE.RingGeometry(8 + i * 5, 8.1 + i * 5, 64);
            const ringMat = new THREE.MeshBasicMaterial({
                color: new THREE.Color().setHSL(0.7 - i * 0.1, 0.8, 0.5),
                transparent: true,
                opacity: 0.08,
                side: THREE.DoubleSide
            });
            const ring = new THREE.Mesh(ringGeom, ringMat);
            ring.rotation.x = Math.PI * 0.3 + i * 0.2;
            ring.rotation.y = i * 0.5;
            this.scene.add(ring);
            this.rings.push(ring);
        }

        // Mouse tracking
        document.addEventListener('mousemove', (e) => {
            this.mouse.x = (e.clientX / window.innerWidth - 0.5) * 2;
            this.mouse.y = -(e.clientY / window.innerHeight - 0.5) * 2;
        });

        window.addEventListener('resize', () => this.resize());

        this.active = true;
        this.animate();
    },

    animate() {
        if (!this.active) return;
        this.animationId = requestAnimationFrame(() => this.animate());

        const time = Date.now() * 0.0005;

        // Rotate particles slowly
        if (this.particles) {
            this.particles.rotation.y = time * 0.1;
            this.particles.rotation.x = Math.sin(time * 0.3) * 0.05;
        }

        // Mouse parallax
        if (this.camera) {
            this.camera.position.x += (this.mouse.x * 3 - this.camera.position.x) * 0.02;
            this.camera.position.y += (this.mouse.y * 3 - this.camera.position.y) * 0.02;
            this.camera.lookAt(this.scene.position);
        }

        // Animate rings
        this.rings.forEach((ring, i) => {
            ring.rotation.z = time * (0.1 + i * 0.05);
            ring.material.opacity = 0.05 + Math.sin(time + i) * 0.03;
        });

        this.renderer.render(this.scene, this.camera);
    },

    resize() {
        if (!this.camera || !this.renderer) return;
        this.camera.aspect = window.innerWidth / window.innerHeight;
        this.camera.updateProjectionMatrix();
        this.renderer.setSize(window.innerWidth, window.innerHeight);
    },

    show() {
        const canvas = document.getElementById('three-canvas');
        if (canvas) canvas.style.display = 'block';
        if (!this.active && this.scene) {
            this.active = true;
            this.animate();
        }
    },

    hide() {
        const canvas = document.getElementById('three-canvas');
        if (canvas) canvas.style.display = 'none';
        this.active = false;
        if (this.animationId) cancelAnimationFrame(this.animationId);
    },

    destroy() {
        this.active = false;
        if (this.animationId) cancelAnimationFrame(this.animationId);
        if (this.renderer) this.renderer.dispose();
    }
};

/* ============================================================
   KANYAMART LANDING PAGE
   ============================================================ */
const LandingPage = {
    render() {
        ThreeScene.show();
        return `
        <div class="landing-page">
            <div class="landing-hero">
                <div class="landing-hero-content">
                    <div class="landing-subtitle">✦ THE FUTURE OF SHOPPING ✦</div>
                    <h1 class="landing-title">KanyaMart</h1>
                    <p class="landing-description">
                        Discover products in a smarter, more immersive marketplace. 
                        Experience shopping reimagined with cutting-edge technology.
                    </p>
                    <div class="landing-buttons">
                        <button class="btn btn-primary btn-lg" onclick="App.navigate('/shop')">
                            🏪 Explore Marketplace
                        </button>
                        <button class="btn btn-secondary btn-lg" onclick="App.navigate('/register')">
                            🚀 Become a Seller
                        </button>
                    </div>
                </div>
            </div>
        </div>`;
    }
};

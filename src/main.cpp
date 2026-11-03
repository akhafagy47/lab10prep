#include <chrono>
#include <iostream>

#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>

int main() {
    // OpenGL settings we request for:
    sf::ContextSettings settings;
    settings.depthBits = 24;   // 24-bit depth buffer
    settings.stencilBits = 8;  // 8-bit stencil buffer
    // Request OpenGL 2.1 for compatibility with both SFML/Graphics AND macOS:
    //  - SFML's Graphics module implementation relies on legacy fixed-function pipeline features
    //     of OpenGL, which means only up to OpenGL 2.1. Any newer version requires Compatibility
    //     profile (i.e., maintaining legacy features).
    //  - macOS has deprecated OpenGL support, meaning for anything newer than 2.1 (i.e., 3.0 and
    //      newer), only Core profile is supported, i.e., REMOVING legacy fixed-function features.
    // As a result, if you want to both support macOS AND use SFML's Graphics features (e.g.,
    // sf::RenderWindow), you can use at latest OpenGL 2.1, which is a nightmare.
    settings.majorVersion = 2;
    settings.minorVersion = 1;
    sf::RenderWindow window(sf::VideoMode({1280, 720}), "Shaders", sf::State::Windowed, settings);
    // Query the settings we actually received (may be different):
    const auto& actualSettings = window.getSettings();
    std::cout << "OpenGL version " << actualSettings.majorVersion << "."
              << actualSettings.minorVersion << " flags: " << actualSettings.attributeFlags << '\n';
    // Fullscreen triangle, as described in the prep.
    sf::VertexArray fullscreenTriangle(sf::PrimitiveType::Triangles, 3);
    fullscreenTriangle[0].position = {-1, -1};  // bottom-left
    fullscreenTriangle[1].position = {3, -1};   // bottom-right
    fullscreenTriangle[2].position = {-1, 3};   // top-left
    fullscreenTriangle[0].texCoords = {0, 0};
    fullscreenTriangle[1].texCoords = {2, 0};
    fullscreenTriangle[2].texCoords = {0, 2};
    // load vertex and fragment shaders:
    sf::Shader shader(std::filesystem::path{"main.vert"}, std::filesystem::path{"main.frag"});
    sf::Shader postProcessShader(std::filesystem::path{"main.vert"},
                                 std::filesystem::path{"blur.frag"});
    // the main shader renders into this texture, which is then blurred into the window:
    sf::RenderTexture preBlurRenderTexture(window.getSize());
    const auto startTime = std::chrono::steady_clock::now();
    while (window.isOpen()) {
        while (auto e = window.pollEvent()) {
            if (e->is<sf::Event::Closed>()) {
                return 0;
            }
            if (const auto* resized = e->getIf<sf::Event::Resized>()) {
                if (!preBlurRenderTexture.resize(resized->size)) {
                    std::cerr << "Failed to resize render texture\n";
                    return 1;
                }
            }
        }
        const sf::Vector2f windowSize(window.getSize());
        // SFML's mouse coords have the origin at the top-left, but gl_FragCoord's is the
        // bottom-left, so flip y:
        const sf::Vector2f mousePos(sf::Mouse::getPosition(window));
        const float elapsedMs =
            std::chrono::duration<float, std::milli>(std::chrono::steady_clock::now() - startTime)
                .count();
        shader.setUniform("windowSize", windowSize);
        shader.setUniform("mousePos", sf::Vector2f(mousePos.x, windowSize.y - mousePos.y));
        shader.setUniform("time", elapsedMs);
        postProcessShader.setUniform("texture", preBlurRenderTexture.getTexture());
        postProcessShader.setUniform("textureSize", sf::Vector2f(preBlurRenderTexture.getSize()));

        preBlurRenderTexture.clear(sf::Color::Black);
        preBlurRenderTexture.draw(fullscreenTriangle, &shader);
        preBlurRenderTexture.display();

        window.clear(sf::Color::Black);
        window.draw(fullscreenTriangle, &postProcessShader);
        window.display();
    }
    return 0;
}

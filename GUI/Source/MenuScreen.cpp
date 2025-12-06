/**
 * @file MenuScreen.cpp
 * @brief Implementarea logicii pentru meniul principal al jocului Pacman.
 * @details Include gestionarea input-ului (mouse/tastatură), randarea grafică și animațiile de fundal.
 */

#include "MenuScreen.hpp"
#include <filesystem>
#include <cmath>

namespace fs = std::filesystem;

namespace Pacman {

    namespace {
        /**
         * @brief Încearcă să încarce un font dintr-o listă de căi comune de sistem.
         *
         * Această funcție este utilă pentru a asigura portabilitatea între Windows, Linux și macOS,
         * căutând fonturi standard (Arial, Segoe UI, etc.) dacă asset-ul local lipsește.
         *
         * @param font Referință către obiectul sf::Font care va fi încărcat.
         * @return true dacă fontul a fost încărcat cu succes, false altfel.
         */
        bool TryLoadFont(sf::Font& font) {
            const char* candidates[] = {
                "assets/arial.ttf", // Prioritizăm fișierul local
                "C:/Windows/Fonts/arial.ttf",
                "C:/Windows/Fonts/segoeui.ttf",
                "C:/Windows/Fonts/tahoma.ttf",
                "C:/Windows/Fonts/verdana.ttf",
                "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
                "/System/Library/Fonts/Helvetica.ttc"
            };

            for (const char* path : candidates) {
                if (fs::exists(path) && font.loadFromFile(path)) {
                    return true;
                }
            }

            return false;
        }
    }

    /**
     * @brief Constructorul meniului.
     * @param listener Interfața care va reacționa la butoanele apăsate (Play/Quit).
     */
    MenuScreen::MenuScreen(std::shared_ptr<IMenuListener> listener)
        : listener_(std::move(listener)), animationTimer_(0.0f) {
    }

    /**
     * @brief Încarcă resursele necesare (fonturi) și inițializează textele.
     * @return true dacă resursele au fost încărcate corect.
     */
    bool MenuScreen::LoadAssets(const std::string& /*assetPath*/) {
        if (!TryLoadFont(font_)) {
            return false; // Nu putem rula fără font
        }

        // Configurare Titlu
        titleText_.setFont(font_);
        titleText_.setString("PAC-MAN");
        titleText_.setCharacterSize(72);
        titleText_.setFillColor(sf::Color::Yellow);
        titleText_.setOutlineColor(sf::Color(200, 150, 0));
        titleText_.setOutlineThickness(3.0f);

        // Configurare buton Play
        playText_.setFont(font_);
        playText_.setString("PLAY");
        playText_.setCharacterSize(48);
        playText_.setFillColor(sf::Color::White);

        // Configurare buton Quit
        quitText_.setFont(font_);
        quitText_.setString("QUIT");
        quitText_.setCharacterSize(48);
        quitText_.setFillColor(sf::Color::White);

        return true;
    }

    /**
     * @brief Actualizează starea internă a meniului (animații).
     * @param deltaTime Timpul scurs de la ultimul frame (în secunde).
     */
    void MenuScreen::Update(float deltaTime) {
        animationTimer_ += deltaTime;
    }

    /**
     * @brief Schimbă selecția curentă (navigare cu tastatura).
     * @param delta +1 pentru jos, -1 pentru sus.
     */
    void MenuScreen::UpdateSelection(int delta) {
        // Folosim formula matematica pentru a cicla intre 0 si 1 (Play si Quit)
        // Adunăm 2 pentru a evita numerele negative în modulo.
        selectedIndex_ = (selectedIndex_ + delta + 2) % 2;
    }

    /**
     * @brief Recalculează pozițiile elementelor pe ecran.
     *
     * Această funcție asigură că meniul rămâne centrat chiar dacă fereastra este redimensionată.
     * De asemenea, aplică efectul de "pulsare" asupra titlului.
     */
    void MenuScreen::UpdateLayout(sf::RenderWindow& window) {
        const float width = static_cast<float>(window.getSize().x);
        const float height = static_cast<float>(window.getSize().y);

        // --- Animație Titlu ---
        // Folosim sin() pentru a mări și micșora titlul în timp
        float pulseScale = 1.0f + 0.05f * std::sin(animationTimer_ * 3.0f);

        sf::FloatRect titleBounds = titleText_.getLocalBounds();
        // Setăm originea în centrul textului pentru scalare corectă
        titleText_.setOrigin(
            titleBounds.left + titleBounds.width / 2.0f,
            titleBounds.top + titleBounds.height / 2.0f
        );
        titleText_.setPosition(width * 0.5f, height * 0.18f);
        titleText_.setScale(pulseScale, pulseScale);

        // --- Poziționare Buton Play ---
        sf::FloatRect playBounds = playText_.getLocalBounds();
        playText_.setOrigin(
            playBounds.left + playBounds.width / 2.0f,
            playBounds.top + playBounds.height / 2.0f
        );
        playText_.setPosition(width * 0.5f, height * 0.45f);

        // --- Poziționare Buton Quit ---
        sf::FloatRect quitBounds = quitText_.getLocalBounds();
        quitText_.setOrigin(
            quitBounds.left + quitBounds.width / 2.0f,
            quitBounds.top + quitBounds.height / 2.0f
        );
        quitText_.setPosition(width * 0.5f, height * 0.65f);

        // --- Calculare zone de click (Hitboxes) ---
        const float padding = 25.0f;
        playRect_ = sf::FloatRect(
            playText_.getPosition().x - playBounds.width / 2.0f - padding,
            playText_.getPosition().y - playBounds.height / 2.0f - padding,
            playBounds.width + padding * 2,
            playBounds.height + padding * 2
        );

        quitRect_ = sf::FloatRect(
            quitText_.getPosition().x - quitBounds.width / 2.0f - padding,
            quitText_.getPosition().y - quitBounds.height / 2.0f - padding,
            quitBounds.width + padding * 2,
            quitBounds.height + padding * 2
        );
    }

    /**
     * @brief Gestionează evenimentele de input (tastatură și mouse).
     * @param event Evenimentul SFML curent.
     */
    void MenuScreen::HandleEvent(const sf::Event& event) {
        // --- Input Tastatură ---
        if (event.type == sf::Event::KeyPressed) {
            // Navigare Sus/Jos
            if (event.key.code == sf::Keyboard::Up || event.key.code == sf::Keyboard::W) {
                UpdateSelection(-1);
            }
            else if (event.key.code == sf::Keyboard::Down || event.key.code == sf::Keyboard::S) {
                UpdateSelection(+1);
            }
            // Confirmare (Enter/Space)
            else if (event.key.code == sf::Keyboard::Enter || event.key.code == sf::Keyboard::Space) {
                if (selectedIndex_ == 0 && listener_) {
                    listener_->OnPlaySelected();
                }
                else if (selectedIndex_ == 1 && listener_) {
                    listener_->OnQuitSelected();
                }
            }
            // Ieșire rapidă cu ESC
            else if (event.key.code == sf::Keyboard::Escape) {
                if (listener_) {
                    listener_->OnQuitSelected();
                }
            }
        }
        // --- Input Mouse Click ---
        else if (event.type == sf::Event::MouseButtonPressed) {
            if (event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2f mousePos(
                    static_cast<float>(event.mouseButton.x),
                    static_cast<float>(event.mouseButton.y)
                );

                if (playRect_.contains(mousePos)) {
                    if (listener_) listener_->OnPlaySelected();
                }
                else if (quitRect_.contains(mousePos)) {
                    if (listener_) listener_->OnQuitSelected();
                }
            }
        }
        // --- Input Mouse Movement (Hover) ---
        else if (event.type == sf::Event::MouseMoved) {
            sf::Vector2f mousePos(
                static_cast<float>(event.mouseMove.x),
                static_cast<float>(event.mouseMove.y)
            );

            // Selectăm automat butonul peste care este mouse-ul
            if (playRect_.contains(mousePos)) {
                selectedIndex_ = 0;
            }
            else if (quitRect_.contains(mousePos)) {
                selectedIndex_ = 1;
            }
        }
        else if (event.type == sf::Event::Closed) {
            requestClose_ = true;
            if (listener_) {
                listener_->OnQuitSelected();
            }
        }
    }

    /**
     * @brief Desenează întregul meniu pe fereastră.
     * @param window Fereastra de randare SFML.
     */
    void MenuScreen::Render(sf::RenderWindow& window) {
        // 1. Desenăm fundalul (gradient simplu din două dreptunghiuri)
        sf::RectangleShape bgTop(sf::Vector2f(window.getSize().x, window.getSize().y / 2.0f));
        bgTop.setFillColor(sf::Color(10, 10, 50));
        bgTop.setPosition(0, 0);
        window.draw(bgTop);

        sf::RectangleShape bgBottom(sf::Vector2f(window.getSize().x, window.getSize().y / 2.0f));
        bgBottom.setFillColor(sf::Color(0, 0, 15));
        bgBottom.setPosition(0, window.getSize().y / 2.0f);
        window.draw(bgBottom);

        // 2. Desenăm efectele de fundal (buline animate)
        DrawBackgroundPellets(window);

        // 3. Recalculăm layout-ul înainte de desenare
        UpdateLayout(window);

        bool playSelected = (selectedIndex_ == 0);
        bool quitSelected = (selectedIndex_ == 1);

        // 4. Setăm culorile textului în funcție de selecție
        playText_.setFillColor(playSelected ? sf::Color::Cyan : sf::Color::White);
        quitText_.setFillColor(quitSelected ? sf::Color::Cyan : sf::Color::White);

        // Adăugăm un outline subtil pentru textul selectat
        if (playSelected) {
            playText_.setOutlineColor(sf::Color(0, 255, 255, 100));
            playText_.setOutlineThickness(2.0f);
        }
        else {
            playText_.setOutlineThickness(0.0f);
        }

        if (quitSelected) {
            quitText_.setOutlineColor(sf::Color(0, 255, 255, 100));
            quitText_.setOutlineThickness(2.0f);
        }
        else {
            quitText_.setOutlineThickness(0.0f);
        }

        // 5. Desenăm butoanele și cutiile lor
        DrawButton(window, playRect_, playSelected);
        DrawButton(window, quitRect_, quitSelected);

        DrawDecorativeBorder(window);

        // 6. Desenăm textele deasupra a tot
        window.draw(titleText_);
        window.draw(playText_);
        window.draw(quitText_);

        window.display();
    }

    /**
     * @brief Helper pentru desenarea unui buton cu efecte vizuale.
     * @param rect Dimensiunile și poziția butonului.
     * @param isSelected Dacă butonul este selectat (mouse hover sau tastatură).
     */
    void MenuScreen::DrawButton(sf::RenderWindow& window, const sf::FloatRect& rect, bool isSelected) {
        // Efect de strălucire (Glow) dacă este selectat
        if (isSelected) {
            sf::RectangleShape glow;
            float glowSize = 8.0f;
            glow.setPosition(rect.left - glowSize, rect.top - glowSize);
            glow.setSize(sf::Vector2f(rect.width + glowSize * 2, rect.height + glowSize * 2));
            glow.setFillColor(sf::Color(0, 150, 150, 30));
            window.draw(glow);
        }

        // Corpul butonului
        sf::RectangleShape background;
        background.setPosition(rect.left, rect.top);
        background.setSize(sf::Vector2f(rect.width, rect.height));
        // Culoare mai deschisă dacă e selectat
        background.setFillColor(isSelected ? sf::Color(20, 40, 60, 180) : sf::Color(15, 15, 35, 150));
        background.setOutlineThickness(3.0f);
        background.setOutlineColor(isSelected ? sf::Color::Cyan : sf::Color(60, 60, 120));
        window.draw(background);

        // Highlight (reflexie mică în colțul stânga-sus)
        sf::RectangleShape highlight;
        highlight.setPosition(rect.left + 3, rect.top + 3);
        highlight.setSize(sf::Vector2f(rect.width - 6, 2));
        highlight.setFillColor(sf::Color(255, 255, 255, isSelected ? 60 : 30));
        window.draw(highlight);
    }

    /**
     * @brief Desenează o grilă de "pellets" (buline) în fundal cu o animație de val.
     * @details Folosește funcția sin() bazată pe poziția în grilă (row/col) și timp pentru a modifica opacitatea.
     */
    void MenuScreen::DrawBackgroundPellets(sf::RenderWindow& window) {
        const int rows = 8;
        const int cols = 14;
        const float spacing = window.getSize().x / (cols + 1.0f);
        const float vSpacing = window.getSize().y / (rows + 1.0f);

        for (int row = 0; row < rows; ++row) {
            for (int col = 0; col < cols; ++col) {
                float x = spacing * (col + 1);
                float y = vSpacing * (row + 1);

                // Animație tip "val" (Wave effect)
                float wave = std::sin(animationTimer_ * 2.0f + col * 0.3f + row * 0.2f);
                // Mapăm valoarea sin (-1 la 1) la o valoare de transparență (Alpha)
                float alpha = 30.0f + wave * 15.0f;

                sf::CircleShape pellet(3.0f);
                pellet.setFillColor(sf::Color(255, 184, 174, static_cast<sf::Uint8>(alpha)));
                pellet.setPosition(x - 3.0f, y - 3.0f);
                window.draw(pellet);
            }
        }
    }

    void MenuScreen::DrawDecorativeBorder(sf::RenderWindow& window) {
        const float thickness = 4.0f;
        const float margin = 20.0f;
        const float width = window.getSize().x - margin * 2;
        const float height = window.getSize().y - margin * 2;

        // Desenăm marginile (stil retro arcade)
        // Top
        sf::RectangleShape topBorder(sf::Vector2f(width, thickness));
        topBorder.setPosition(margin, margin);
        topBorder.setFillColor(sf::Color(33, 33, 222, 150));
        window.draw(topBorder);

        // Bottom
        sf::RectangleShape bottomBorder(sf::Vector2f(width, thickness));
        bottomBorder.setPosition(margin, window.getSize().y - margin - thickness);
        bottomBorder.setFillColor(sf::Color(33, 33, 222, 150));
        window.draw(bottomBorder);

        // Left
        sf::RectangleShape leftBorder(sf::Vector2f(thickness, height));
        leftBorder.setPosition(margin, margin);
        leftBorder.setFillColor(sf::Color(33, 33, 222, 150));
        window.draw(leftBorder);

        // Right
        sf::RectangleShape rightBorder(sf::Vector2f(thickness, height));
        rightBorder.setPosition(window.getSize().x - margin - thickness, margin);
        rightBorder.setFillColor(sf::Color(33, 33, 222, 150));
        window.draw(rightBorder);
    }

}
#ifndef UI_HPP
#define UI_HPP

#include <SFML/Graphics.hpp>
using namespace sf;
class UI {
public:
    RectangleShape hpBar;
    RectangleShape hpBarBack;
    RectangleShape bossHpBar;
    RectangleShape bossHpBarBack;

    UI(Vector2u windowSize, int playerHP, int bossHP) {
        hpBarBack.setSize({200.f, 20.f});
        hpBarBack.setFillColor(Color(100, 100, 100));
        hpBarBack.setPosition({20.f, 20.f});

        hpBar.setSize({200.f * (float)playerHP / 20.f, 20.f});
        hpBar.setFillColor(Color::Red);
        hpBar.setPosition({20.f, 20.f});

        bossHpBarBack.setSize({200.f, 20.f});
        bossHpBarBack.setFillColor(Color(100, 100, 100));
        bossHpBarBack.setPosition({windowSize.x - 220.f, 20.f});

        bossHpBar.setSize({200.f * (float)bossHP / 20.f, 20.f});
        bossHpBar.setFillColor(Color::Green);
        bossHpBar.setPosition({windowSize.x - 220.f, 20.f});
    }

    void update(int playerHP, int bossHP) {
        hpBar.setSize({200.f * (float)playerHP / 20.f, 20.f});
        bossHpBar.setSize({200.f * (float)bossHP / 20.f, 20.f});
    }
};

#endif

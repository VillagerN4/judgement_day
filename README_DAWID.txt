

<section id="HERO">
            <h2>Klasa Bohatera</h2>
            <p>Zawiera klasy naszego bohatera i pocisków którymi strzela</p>
            <ul>
                <li><strong>Projectile 2</strong> – tworzy pociski strzelane przez gracza przy bosie Eufemi</li>
                <li><strong>Update, updateProjectiles2 i draw</strong> – logika resetowania pocisków  i rysowanie obiektów (gracza, pocisków)</li>
                <li><strong>Hero</strong> – Tworzenie gracza </li>
                <li><strong>handleMovement, handleJumping</strong> – obsługa poruszania się gracza</li>
                <li><strong>shoot</strong> – poruszanie się pocisków </li>
                <li><strong>takeDamage</strong> – dostawanie obrażeń gracza</li>
            </ul>
</section>
<section id="BOSSES">
            <h2>Klasa Eufemi</h2>
            <p>Zawiera tworzenie Bossa</p>
            <ul>
                <li><strong>Konstruktor Boss</strong> – tworzy obiekt Bossa i ustawia teksturę i życie</li>
                <li><strong>getHP,takeDamage i isAlive </strong> – sprawdza życie bossa i zadaje mu obrażenia </li>
                <li><strong>phaseTwoAttack</strong> – Druga faza bossa polega na strzelaniu pociskami które lecą w stronę gracza </li>
            </ul>
</section>
<section id="CRYSTAL">
            <h2>Klasa kryształu</h2>
            <p>Zawiera tworzenie kryształów które strzelają w gracz</p>
            <ul>
                <li><strong>Konstruktor Crystal</strong> – tworzy obiekt kryształu, ustawia teksturę, życie i okrąg według które ma się obracać </li>
                <li><strong>Update</strong> – Aktualizacja pozycji kryształu ze względu na obracanie</li>
                <li><strong>takeDamage, isAlive i getHP</strong> – otrzymuje obrażenia, sprawdza czy żyje i pobiera Hp </li>
                <li><strong>getGlobalBounds</strong> - pobiera hitbox kryształu</li>
            </ul>
</section>

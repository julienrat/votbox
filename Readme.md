Vot'Box - boitier de vote 
=

----
== Fonctionnement
=== Connection à un réseau Wifi
Au démarrage si le boitier ne reconnait pas de wifi, les 2 leds (rouge et vertes) se metent à clignotter signalant que ce dernier est en mode de configuration.

Afin de connecter celui-ci à un réseau Wifi, il faut se connecter sur le SSID (Vot_box_XXXXXX ) et se rendre à l'adresse 192.168.4.1 pour accéder à l'interface de configuration.

[[Image(accueil.png)]]

Cliquez sur ** Configure Wifi


[[Image(selection.png)]]

Sélectionnez le réseau puis entrez votre mot de passe

Pour effacer les réseaux enregistrés, allumez le boitier en maintenant la touche ** # ** enfoncée.
=== Utilisation
A l'allumage du boitier, la led verte se met à clignotter indiquant que celui-ci est en train de se connecter. Si celui-ci trouve son réseau, la led arrete de clignotter et devient fixe.

Pour voter, faites votre choix (chiffres et lettres). à la pression sur une touche, la led rouge se met à clignotter indiquant que le vote est mémorisé. Pour modifier en cas d'erreur, appuyez sur la touche * . La led rouge s'eteint et permet de sélectionner une autre réponse.

Pour valider et envoyer votre vote au serveur, appuyez sur la touche #. La led rouge arrete de clignotter et devient fixe. Sinon, le boitier se met en point d'accès (les leds rouges et vertes clignottent)

=== Remise à zero
Pour permettre un autre vote, l'animateur doit réinitialiser les boitiers en envoyant la requette : http://adresse_ip/RAZ la led rouge s'éteint et permet alors un nouveau vote.

=== Messages

|| [[Image(leds_ap.png)]] || Le boitier ne parvient pas à se connecter, Mode configuration en point d'accès ||
|| [[Image(leds_connect.png)]] || Le boitier est en train de se connecter ||
|| [[Image(leds_ok.png)]] || Le boitier est connecté et est prêt à être utilisé ||
|| [[Image(leds_vote_mem.png)]] ||Vote enregistré dans la mémoire mais pas envoyé au serveur||
|| [[Image(leds_voted.png)]] ||Vote effectué, attente de remise à zero||


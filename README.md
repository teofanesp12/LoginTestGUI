# LoginTestGUI
**Login Test GUI**

INSTALL
-------
   Dep:

      libpq-fe, gtk-+3
   Debian Install:

      apt-get install libpq-dev libgtk-3-dev
BUILDING
--------
   Para Compilar, Abra o Terminal e execute o Comando:

    make
RUN
---
   Você Pode Usar o Comando:

    make run
   Ou:

    ./login
TEST
----
   Você pode testar o Software com:

    apt-get install postgesql pgadmin3
   
   Sera necessario criar a tabela users, com colunas login e password

   No Caso de Usar o UBUNTU: `PGADMIN III`_

HELP
----
   Para Exibir a Ajuda, Use o Comando:

    make help

.. _PGADMIN III: apt:pgadmin3

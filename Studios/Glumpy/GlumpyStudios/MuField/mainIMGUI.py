# -*- coding: utf-8 -*-


from MuFieldApp import MuFieldApp
from IMGUIPlayer import ImGuiPlayer


if __name__ == "__main__":
    # Se o player nao for instanciado primeiro,
    # contexto nao eh inicializado direito e da problema com screen buffers
    player = ImGuiPlayer()

    app = MuFieldApp()

    player.run(app)

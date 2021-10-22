import ViewerApp
from IMGUIPlayer import ImGuiPlayer


if __name__ == '__main__':
    player = ImGuiPlayer()

    viewer = ViewerApp.Field2DViewer()
    player.run(viewer)

import MSViewer
from IMGUIPlayer import ImGuiPlayer


if __name__ == '__main__':
    player = ImGuiPlayer()

    viewer = MSViewer.StringsEqsViewer(GridDim=128, SpaceDim=4)
    player.run(viewer)

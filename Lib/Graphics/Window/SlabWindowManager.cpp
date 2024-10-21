//
// Created by joao on 10/19/24.
//

#include "SlabWindowManager.h"
#include "Decorator.h"
#include "WindowStyles.h"
#include "Graphics/SlabGraphics.h"
#include <functional>

namespace Slab::Graphics {
    static Decorator decorate;

    template <typename T, typename Predicate>
    typename std::list<T>::iterator find_first_if(std::list<T>& lst, Predicate pred) {
        for(auto it = lst.begin(); it != lst.end(); ++it) {
            if(pred(*it)) {
                return it;
            }
        }
        return lst.end();
    }

    // Function to move the first occurrence of a value to the front
    template <typename T>
    bool move_to_front(std::list<T>& lst, const T& value) {
        auto it = std::find(lst.begin(), lst.end(), value);
        if (it != lst.end()) {
            lst.splice(lst.begin(), lst, it);
            return true;
        }
        return false;
    }

    template <typename T>
    bool move_to_front(std::list<T>& list, const typename std::list<T>::iterator& it) {
        if (it != list.end()) {
            list.splice(list.begin(), list, it);
            return true;
        }
        return false;
    }

    void SlabWindowManager::addSlabWindow(Pointer<SlabWindow> slab_window) {
        slab_windows.push_back(slab_window);

        if(focused == nullptr) focused = slab_window;
    }

    bool SlabWindowManager::notifyKeyboard(KeyMap key, KeyState state, ModKeys modKeys) {
        if(focused == nullptr) return false;

        return focused->notifyKeyboard(key, state, modKeys);
    }

    bool SlabWindowManager::notifyMouseButton(MouseButton button, KeyState state, ModKeys keys) {
        if(focused == nullptr) return false;

        if(button==MouseButton_LEFT && state==Press) {
            auto mouse_state = Graphics::GetGraphicsBackend().getMouseState();

            auto first = find_first_if(slab_windows, [mouse_state](const Pointer<SlabWindow> &window){
                return window->isMouseIn() || decorate.isMouseOverGrabRegion(*window, mouse_state.x, mouse_state.y);
            });

            if(first != slab_windows.end()) {
                focused = *first;
                if(decorate.isMouseOverGrabRegion(*focused, mouse_state.x, mouse_state.y)) {
                    grabbed = focused;
                } else {
                    grabbed = nullptr;
                }

                move_to_front(slab_windows, first);
            }
        } else if(state==Release) grabbed = nullptr;

        return focused->notifyMouseButton(button, state, keys);
    }

    bool SlabWindowManager::notifyMouseMotion(int x, int y, int dx, int dy) {
        if(focused == nullptr) return false;

        if(grabbed != nullptr) {
            grabbed->setx(x);
            grabbed->sety(y);

            return true;
        }

        return focused->notifyMouseMotion(x, y);
    }

    bool SlabWindowManager::notifyMouseWheel(double dx, double dy) {
        if(focused == nullptr) return false;

        return focused->notifyMouseWheel(dx, dy);
    }

    bool SlabWindowManager::notifyFilesDropped(StrVector paths) {
        if(focused == nullptr) return false;

        return false;
    }

    bool SlabWindowManager::notifySystemWindowReshape(int w, int h) {
        decorate.setSystemWindowShape(w, h);

        for(auto &slab_window : slab_windows) {
            slab_window->setupParentSystemWindowHeight(h);

            if (slab_window->wantsFullscreen()) {
                slab_window->setx(0);
                slab_window->sety(Graphics::menuHeight);
                slab_window->notifyReshape(w, h - Graphics::menuHeight);
            }
        }

        return false;
    }

    bool SlabWindowManager::notifyRender() {
        if(focused == nullptr) return false;

        for (auto it = slab_windows.rbegin(); it != slab_windows.rend(); ++it) {
            decorate(**it);
            (*it)->draw();
        }

        // for(auto &slab_window : slab_windows) {
        //     decorate(*slab_window);
        //     slab_window->draw();
        // }

        return true;
    }



} // Slab::Graphics
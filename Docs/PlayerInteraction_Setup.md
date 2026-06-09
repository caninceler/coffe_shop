# Player Interaction Setup

First-person interaction is now native C++.

## Native classes

- `ACoffeeShopPlayerCharacter`
  - First-person camera.
  - WASD movement, mouse look.
  - `E` runs an interaction trace from the camera.
  - Default `InteractionDistance` is `350`.

- `UCoffeeShopInteractable` / `ICoffeeShopInteractable`
  - Native interaction interface for gameplay-critical interactables.
  - Blueprint interactables can still use the asset-pack `BPI_Interact` later for visuals/prompts.

- `ACoffeeShopGameMode`
  - Uses `ACoffeeShopPlayerCharacter` as `DefaultPawnClass`.

- `ACoffeeShopCustomerServicePoint`
  - Implements native interaction.
  - Looking at the service point and pressing `E` calls `AcceptNextPayment()`.

## Editor setup

1. Restart the editor after adding these new C++ classes.
2. Open `Coffe_Shop_Demo_Map`.
3. Open `World Settings`.
4. Set `GameMode Override` to `CoffeeShopGameMode`.
5. Make sure the level has one `Player Start` behind/near the counter.
6. Select `BP_CustomerServicePoint`.
7. Make sure the visible/colliding actor or mesh can block `Visibility` traces.
8. Press Play.
9. Look at the service point/counter area.
10. Press `E`.
11. The first customer waiting in the payment queue should switch to pickup flow.

If `E` does nothing, test closer to the service point first. Then check collision: the actor being looked at must block `Visibility`, or the trace cannot hit it.

# Customer NPC Setup

This is the first clean slice for customers. Code owns state and references; Blueprint owns visuals, collision, animation, and asset wiring.

## Native classes

- `ACoffeeShopCustomerCharacter`
  - Create `BP_Customer` from this.
  - Replace skeletal mesh, animation blueprint, capsule size, collision, and movement settings in Blueprint.
  - Assign `BehaviorTreeAsset` to `BT_Customer`.
  - Set `OrderRequest.DrinkId` for the first test order, for example `BasicCoffee`.

- `ACoffeeShopCustomerAIController`
  - Default controller for `ACoffeeShopCustomerCharacter`.
  - Runs the assigned Behavior Tree on possess.
  - Syncs these Blackboard keys:
    - `VisitState` as Enum
    - `ServicePoint` as Object
    - `Seat` as Object
    - `TargetActor` as Object
    - `TargetLocation` as Vector

- `ACoffeeShopCustomerSpawnPoint`
  - Place at the entrance/door.
  - Set `CustomerClass` to `BP_Customer`.
  - Set `DefaultServicePoint` to the counter service point.
  - Enable `bAutoSpawn` to spawn customers automatically.
  - Use `MinAutoSpawnDelay` and `MaxAutoSpawnDelay` for the random spawn interval. The first prototype uses 5-15 seconds.
  - `SpawnCustomer()` remains available for debug keys, level Blueprint tests, or a later manager.

- `ACoffeeShopCustomerServicePoint`
  - Place near the cash register.
  - Move `QueuePoint`, `PaymentPoint`, and `PickupPoint` components in the Blueprint/editor.
  - `PaymentPoint` is where the customer orders/pays.
  - `PickupPoint` is where the customer waits for the prepared drink.

- `ACoffeeShopCustomerSeat`
  - Place one per usable chair.
  - Move `SitPoint` to where the customer should sit and face.
  - `ClaimSeat` prevents two customers from using the same seat.

## First Behavior Tree shape

Create these editor assets manually:

- `BB_Customer`
- `BT_Customer`
- Optional Blueprint BT tasks for the first prototype
- Native task `CoffeeShopBTTask_SetTargetLocationFromPaymentPoint`
- Native task `CoffeeShopBTTask_SubmitCustomerOrder`
- Native task `CoffeeShopBTTask_SetTargetLocationFromPickupPoint`

Suggested Blackboard keys:

| Key | Type | Notes |
| --- | --- | --- |
| `VisitState` | Enum | Use `ECoffeeShopCustomerVisitState`. |
| `ServicePoint` | Object | Base class `ACoffeeShopCustomerServicePoint`. |
| `Seat` | Object | Base class `ACoffeeShopCustomerSeat`. |
| `TargetActor` | Object | Generic current move/focus target. |
| `TargetLocation` | Vector | Generic current move location. |

Suggested first BT flow:

1. Run `Set Target Location From Payment Point`.
2. Move to `TargetLocation`.
3. Run `Submit Customer Order`.
4. Run `Set Target Location From Pickup Point`.
5. Move to `TargetLocation`.
6. Wait calmly for the drink to be served.
7. Later: find an available `ACoffeeShopCustomerSeat`.
8. Later: call `ClaimSeat()` on the customer.
9. Later: move to `Seat.SitPoint`.
10. Later: set state to `Seated`.
11. Later: play sit/idle animation in `BP_Customer`.

Keep this cozy: if no seat is available, the customer should wait calmly or leave without penalty.

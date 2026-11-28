# MayDay-404 — A Real-Time ATC Simulator

CL2001 Data Structures — BCS 3, Section CS-3 E, Fall 2026 semester project.

Course Instructor: Ma'am Zainab Noor

## Team members

| Student | Roll Number | Work description |
|---|---|---|
| Afham Ayub | 25F-0651 | Module 8, Module 9, Module 10 (Dijkstra routing, Kruskal MST, game loop integration) |
| Hijab Habib | 25F-0518 | Module 4, Module 6, Module 7, Module 11 (BST, airspace graph, BFS/DFS, file persistence/leaderboard) |
| Abdullah Sultan | 25F-0716 | Module 1, Module 2, Module 3, Module 5 (Phase 1 registry/hash table/dispatch/undo, AVL rotations) |

## Build and run

Visual Studio 2022/2026, Developer Command Prompt, from the project folder:

```
cl /EHsc /W3 /Fe:MayDay404.exe main.cpp Registration.cpp EmergencyLanding.cpp CommandHistory.cpp BSTflight.cpp AVLflight.cpp SpaceGraph.cpp RadarSweep.cpp RouteFinder.cpp RadarMST.cpp LeaderBoard.cpp GameEngine.cpp
MayDay404.exe
```

Inside the IDE: create an empty C++ console project, add every `.h` and `.cpp` file, and
keep the nine `.txt` files in the working directory (the project folder), because the game
reads them with relative paths.

## Files

| File | Module | Contents |
|---|---|---|
| `main.cpp` | 10 | Main menu and one game engine per New Game |
| `Registration.h/.cpp` | 1 | Flight and crew records, singly linked registry, hash table with linear probing |
| `EmergencyLanding.h/.cpp` | 2 | Circular-array queue for normal arrivals, min-heap for emergencies |
| `CommandHistory.h/.cpp` | 3 | Doubly linked command log, snapshot stack for undo |
| `BSTflight.h/.cpp` | 4 | BST flight manifest keyed by callsign |
| `AVLflight.h/.cpp` | 5 | Self-balancing AVL callsign index with all four rotations |
| `SpaceGraph.h/.cpp` | 6 | Airspace graph: adjacency list and adjacency matrix |
| `RadarSweep.h/.cpp` | 7 | BFS and DFS over custom queue and stack, missing-flight search |
| `RouteFinder.h/.cpp` | 8 | Dijkstra with a custom min-heap and parent-pointer tracing |
| `RadarMST.h/.cpp` | 9 | Kruskal MST with union-find and a hand written insertion sort |
| `GameEngine.h/.cpp` | 10 | Tick loop, radar UI, credibility, escalation, missing-flights list |
| `LeaderBoard.h/.cpp` | 11 | All file I/O and the persistent leaderboard |
| `flights.txt`, `crew.txt`, `airspace.txt`, `scenario.txt`, `rules.txt`, `about.txt`, `gamemenu.txt`, `mainmenu.txt` | — | Input files read at startup |
| `leaderboard.txt` | 11 | Appended at game end |

No STL container, STL algorithm or library sort is used. Every data structure is written
from scratch with `new` / `delete`; only `std::string`, `<iostream>`, `<fstream>`,
`<cstdlib>` and `<ctime>` are used from the standard library.

## Function audit against the manual

Every function in the project traces back to a named operation, a game-menu option or a rule of
the manual. The table below is that mapping, so that each line of code can be justified during
the viva. Private helpers that only implement the mechanics of their own structure (heapify,
rotations, union-find, recursive tree walks, growth of a dynamic array) are listed with the
operation they serve.

### Module 1 — `Registration` (and `FlightHashTable`)

| Function | Required by |
|---|---|
| `addFlight`, `removeFlight`, `updateAllFlights`, `displayAllFlights`, `displayAllCrew`, `addCrew` | §4.1.1 operation list, word for word |
| `insert`, `find`, `remove`, `displayStats` | §4.1.1 hash operation list |
| `hashFunction` | §4.1.1 "Design the hash function" |
| `createFlight` | the `new` allocation of the flight records that §4.1.1 requires |
| `findFlight` | §4.1.1 "The hash table makes lookup near-instant" and menu option 03 |
| `getPilotName` | §6.2 "Pilot names show up in radar display and command logs" |
| `getHead`, `getFlightCount`, `displayHashStats` | §4.4.1 step 5, "show radar, flight count" |
| destructor | §4.1.1 "Concepts used: … destructor" |

### Module 2 — `EmergencyLanding`

| Function | Required by |
|---|---|
| `enqueueNormal`, `dequeueNormal`, `frontNormal`, `isEmptyNormal`, `displayNormalQueue` | §4.1.2 queue operation list |
| `insertEmergency`, `extractMin`, `peekMin`, `isEmptyHeap`, `displayHeap` | §4.1.2 heap operation list |
| `processNextLanding` | §4.1.2 "if the normal queue is non-empty, dequeue from it. Otherwise, extract the highest-priority flight from the heap" |
| `hasHigherPriority` | §4.1.2 "The heap comparison must consult both priorityLevel and arrivalTick" |
| `heapifyUp`, `heapifyDown`, `swapNodes`, `growQueue`, `growHeap` | mechanics of the custom queue and min-heap |
| `getEmergencyCount` | §4.4.1 step 5, "show … pending emergencies" |
| `removeFlight` | memory safety, see deviation 8 |

### Module 3 — `CommandHistory`

| Function | Required by |
|---|---|
| `logCommand`, `pushSnapshot`, `undo`, `displayHistory`, `displayLastCommand` | §4.1.3 operation list, word for word; snapshots are pushed by menu options 07, 08, 09 and 10, see deviation 10 |
| `captureState`, `restoreState` | the snapshot of "the affected flight's callsign and its prior state (runway, priority, sector, status)" |
| `invalidateSnapshots` | memory safety, see deviation 8 |

### Module 4 — `BSTflight` and Module 5 — `AVLflight`

| Function | Required by |
|---|---|
| `insert`, `search`, `inorderTraversal`, `height`, `destroyTree` | §4.2.1 operation list; §4.2.2 keeps "the same interface" |
| `rotateLeft`, `rotateRight`, `rotateLeftRight`, `rotateRightLeft` | §4.2.2 "as private helpers" |
| `remove` | §4.4.1 step 7, "remove the flight from register, hash table, BST, AVL" |
| `insertNode`, `searchNode`, `removeNode`, `findMinNode`, `inorderPrint`, `heightOf`, `nodeHeight`, `balanceFactor`, `refreshHeight`, `rebalance`, `destroy` | recursion and balancing mechanics of the two trees |

### Module 6 — `SpaceGraph`

| Function | Required by |
|---|---|
| `addSector`, `addCorridor`, `getWeight`, `displayAdjacencyList`, `displayAdjacencyMatrix`, `getSectorCount` | §4.3.1 operation list, word for word |
| `removeCorridor` | §7.2 cyber alert, "one corridor is disabled (removed from graph)" |
| `isRunway`, `isEmergencyRunway` | §6.3, "Only Runway 09 and 12 have emergency landing capabilities… penalty (-10)" |
| `findSector`, `getSectorName`, `getNeighbours`, `toLower` | what `addSector(name) -> index` implies; see deviation 9 |
| `grow`, destructor | §4.3.1 "Concepts used: … DMA" |

### Module 7 — `RadarSweep` (with `SectorQueue`, `SectorStack`)

| Function | Required by |
|---|---|
| `bfsFrom`, `dfsFrom`, `isReachable`, `searchMissingFlight` | §4.3.2 operation list, word for word |
| `displayWithinHops` | §4.3.2 "Check which sectors are within 2 hops of the Control Tower" |
| `SectorQueue`, `SectorStack` | §4.3.2 "BFS: Must use a custom queue… DFS: Must use a custom stack" |

### Module 8 — `RouteFinder`

| Function | Required by |
|---|---|
| `findFastestRoute`, `printRoute` | §4.3.3 operation list, word for word |
| `DistanceHeap` (`insert`, `extractMin`, `isEmpty`, `heapifyUp`, `heapifyDown`, `swapEntries`) | §4.3.3 "Priority selection inside Dijkstra must use a custom min-heap" |
| parent array and the trace loop inside `findFastestRoute` | §4.3.3 "Complete Parent-Pointer tracing is required" |
| `releaseRoute` | frees the route array allocated with `new` |

### Module 9 — `RadarMST`

| Function | Required by |
|---|---|
| `computeMST`, `displayMST`, `verifyConnectivity` | §4.3.4 operation list, word for word |
| `makeSets`, `findSet`, `unionSets`, `releaseSets` | §4.3.4 "union-find … must be implemented" |
| `sortEdges` | §4.3.4 "edge sorting must be implemented (no std::sort)" |
| `countConnected` | §4.3.4 "check that removing any MST edge disconnects the network" |

### Module 10 — `GameEngine` (and `MissingFlightList`)

| Function | Required by |
|---|---|
| `newGame`, `askCroName` | §4.4.1 "On New Game", non-empty name without space, underscore or dot |
| `loadAllFiles` | §4.4.1 "On Startup", the seven input files |
| `gameLoop` | §4.4.1 "Loop structure (each iteration = 1 turn = 1 tick = 30 simulated seconds)" |
| `displayState` | §4.4.1 step 5, radar, flight count, fuel warnings, pending emergencies, credibility |
| `handleAction` | §7.3 game menu, options 01–24 |
| `actionRegisterAircraft` … `actionCompareTreeHeights` | one function per menu option 01–22 |
| `advanceTick`, `moveFlights`, `landFlight`, `crashFlight` | §4.4.1 steps 1 and 7 |
| `escalateEmergencies` | §4.4.1 step 2 and Table 7 escalation windows |
| `escalateCyberAlert` | §7.2 cyber alert rules |
| `escalateMissingFlights` | §4.4.1 step 4, Restricted Zone breach |
| `spawnScheduledFlights` | §4.4.1 step 3, automatic spawn |
| `applyScenarioEvents`, `eventHijack`, `eventCyber`, `eventFuel`, `eventMissing`, `eventWar` | the five event types of `scenario.txt` |
| `resolveTargetFlight` | §4.4.1 "Randomly map between scenario.txt events and active flights" |
| `dispatchFlight` | §4.4.1 step 3, "dispatched to the queue or heap based on its priority" |
| `assignRoute` | §4.4.1 step 1, "Move one step along its precomputed route" |
| `removeFromAllStructures` | §4.4.1 step 7 |
| `gainCredibility`, `loseCredibility`, `wrongCommand` | §7.2 credibility gains and penalties |
| `penaltyValue`, `routingCost` | §7.2 degraded Control Tower, 2 actions and an extra −5 |
| `windowForPriority`, `priorityLabel` | Table 7 priorities, labels and escalation windows |
| `triggerLose`, `endGame` | §4.4.1 step 8, win/lose modal box |
| `MissingFlightList` (`add`, `findBySector`, `getHead`, `display`, `getPendingCount`) | §4.4.1 step 4, "They live in their own list" |
| `readLine`, `readNumber` | §2 "Handle invalid input and invalid moves through cout messages" |

### Module 11 — `LeaderBoard` (and `ScenarioList`)

| Function | Required by |
|---|---|
| `loadFlights`, `loadCrew`, `loadAirspace`, `loadScenario`, `saveResult`, `displayLeaderboard` | §4.4.2 operation list, word for word |
| `loadRules`, `defaultRules` | §4.4.1 "Read rules.txt… Fall back to defaults if the file is missing or corrupt" |
| `displayTextFile` | §4.4.1 reading of `about.txt`, `gamemenu.txt`, and §7.1 main menu |
| `splitLine`, `trim`, `toInt` | §4.4.2 "Concepts used: File I/O, string parsing" |
| `ranksHigher` | §7.2 "Leaderboard Ranking" order |
| `todayAsString` | the `Date` column of the leaderboard format |
| `ScenarioList` (`add`, `getHead`, `getCount`) | §4.4.2 "loadScenario(filename) [read the spawn schedule]" |

### `main.cpp`

| Function | Required by |
|---|---|
| `mainMenu` | §7.1 "Displayed when the game starts and ends. Can be written as cout in mainMenu( )" |
| `main` | the five main-menu choices and one fresh game engine per New Game |

Six operations are implemented because the manual lists them, although no menu option calls
them: `frontNormal`, `peekMin`, `isEmptyHeap`, `BSTflight::search`, `AVLflight::search` and both
`destroyTree` functions.

## Data-file changes made by this team

* `crew.txt` — added the crew member **Mr. Karim Kardar** (attendant, BB-940), as the manual's
  pilot table (§6.2) asks for, and gave the duplicated employee ID `EBB91` of the sample file a
  unique value (`EBB92`) so that crew IDs stay unique.
* `airspace.txt`, `flights.txt`, `scenario.txt`, `rules.txt`, `about.txt`, `gamemenu.txt`,
  `mainmenu.txt`, `leaderboard.txt` — used as given in the sample file, with no sector, flight
  or corridor added beyond what the manual's own tables and sample files list.

## Deviations from the manual, with justification

1. **Corridors are undirected.** `airspace.txt` lists each corridor once, but aircraft must be
   able to fly a corridor in both directions for BFS, DFS, Dijkstra and the MST to behave as
   the manual describes, so every row of `airspace.txt` creates an edge in both directions.
2. **Flights move only after a clearance.** The manual says every flight advances one step per
   tick, while its sample output shows flights waiting in their sector until the CRO clears them.
   This implementation follows the sample output: an uncleared flight holds in its sector and
   still burns fuel, a cleared flight follows its Dijkstra route.
3. **Cyber escalation uses the section 7.2 timings** (2 ticks → one corridor removed from the
   graph, 3 ticks → Control Tower degraded, so routing commands cost 2 actions and every
   penalty is 5 points harsher). A cyber alert on the Control Tower itself that is not countered
   inside its window is the "cyber breach" lose condition.
4. **Scheduled arrivals come from `flights.txt`.** `scenario.txt` has no field for a new flight's
   data, so a flight spawns at its own `ArrivalTick` (NA-640 and NA-429 at tick 25) and
   `scenario.txt` drives the emergencies.
5. **Passenger casualty costs 10 credibility.** The manual lists the casualty consequence for a
   medical emergency but no value, so the non-emergency-runway penalty value is reused.
6. **Wrong-command penalty.** The −5 penalty is charged for the cases the manual lists
   (invalid menu choice, non-existent flight or runway, a clearance the flight cannot fly,
   escalating an ongoing emergency, countering a cyber alert in the wrong sector, searching for
   a missing flight below the credibility threshold). A purely informational empty result, such
   as "no arrival is waiting", only prints an error message and costs nothing.

7. **Two helper operations exist only for memory safety.** The manual's step 7 removes a landed
   flight from the registry, the hash table, the BST and the AVL, but the record is also pointed
   at by the dispatch structures and by the undo snapshots, so `EmergencyLanding::removeFlight`
   and `CommandHistory::invalidateSnapshots` drop those pointers before the record is deleted.
   Without them the queue, the heap or an undo would read freed memory.
8. **Sector names are matched by exact name first, then case-insensitively.** This is what lets
   the CRO type `17` for `Runway 17 (South)`, as the manual's own sample output shows.
9. **Option 11 "Counter Cyber Attack" is logged but not undoable.** §7.3 lists options 07–11 as
   undoable, but §4.1.3 defines a stack node as "the affected flight's callsign and its prior
   state (runway, priority, sector, status)", and a cyber alert has no affected flight, so there
   is no state of that shape to push. Options 07, 08, 09 and 10 are undoable.
10. **The DFS check "is Runway 27 reachable from Alpha Tree" is not implemented.** §4.3.2 names
   Runway 27, but §6.3 and Table 4 define only Runway 04, 09, 12 and 17, so Runway 27 is not a
   vertex of the airspace. Option 18 runs the other §4.3.2 check instead, the path from the
   source sector to the Restricted Zone.
11. **Corridor weights are read as ticks.** §4.3.1 calls an edge weight "travel time in seconds"
   while Table 4 and `airspace.txt` use ticks (`WeightTicks`); the sample data is followed.
12. **The input files are read when New Game starts**, not before the main menu as in §4.4.1, so
   that every game begins from the files as they are on disk. `about.txt` and `gamemenu.txt` are
   read each time they are displayed.

## Known limitations

1. The game is single player and console only; there is no save or resume of a game in progress,
   only the persistent leaderboard.
2. The spawn schedule and the threat tables (hijacker groups, cyber groups, missing flights) are
   limited to the entries of the manual's tables; new entity types need a code change.
3. Routes are recomputed only when a flight is cleared or rerouted, so a corridor disabled by a
   cyber attack is noticed by an airborne flight only when it reaches its next sector.
4. The adjacency matrix is a dense 2D array, so a very large airspace would use far more memory
   than the adjacency list alone.
5. Undo restores the flight state saved before a command but does not rewind the tick counter,
   the fuel already burnt or the credibility already paid.

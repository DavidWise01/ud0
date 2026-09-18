/-
  Quant.root.0

  Standalone Lean 4 formalization of the frozen Quant.root.0 primitive.

  Model commitments proved below:
    * two half-weights fuse to one full prime;
    * ((4 * 4) ^ 4) = 65,536 = 2 ^ 16;
    * two phase-linked 16-bit cubis contain one independent state;
    * "|||| /\\\\\\\\/ |||| /\\" has 16 directed strokes;
    * the diodic reduction is 16 -> 4 -> 2 -> 1 -> 0;
    * soft closure retains the payload in a sealed Sapphon.

  No physics claim is made here.  These are exact invariants of the symbolic
  transition system named Quant.root.0.
-/

namespace QuantRoot0

def canonicalName : String := "Quant.root.0"

/- -------------------------------------------------------------------------
   Prime: .5 x 2 = 1, represented without floating-point approximation.
   ------------------------------------------------------------------------- -/

inductive Weight where
  | half
  | full
deriving DecidableEq, Repr

def fuseWeight : Weight → Weight → Option Weight
  | .half, .half => some .full
  | _, _ => none

theorem two_halves_make_one :
    fuseWeight .half .half = some .full := by
  rfl

/- -------------------------------------------------------------------------
   Full 16-bit cubi capacity.
   ------------------------------------------------------------------------- -/

theorem cubi_capacity_decimal :
    ((4 * 4 : Nat) ^ 4) = 65536 := by
  decide

theorem cubi_capacity_binary :
    ((4 * 4 : Nat) ^ 4) = 2 ^ 16 := by
  decide

theorem sixteen_bits_have_65536_states :
    (2 : Nat) ^ 16 = 65536 := by
  decide

abbrev Cubi16 := Fin (2 ^ 16)

/- A phase map is explicitly reversible. -/
structure PhaseMap where
  forward : Cubi16 → Cubi16
  backward : Cubi16 → Cubi16
  backward_forward : ∀ x, backward (forward x) = x
  forward_backward : ∀ x, forward (backward x) = x

/- The right cubi is a full view, but it is determined by the left cubi. -/
structure Entangled (phase : PhaseMap) where
  left : Cubi16
  right : Cubi16
  linked : right = phase.forward left

def entangle (phase : PhaseMap) (state : Cubi16) : Entangled phase where
  left := state
  right := phase.forward state
  linked := rfl

def independentState {phase : PhaseMap} (pair : Entangled phase) : Cubi16 :=
  pair.left

theorem independent_entangle
    (phase : PhaseMap) (state : Cubi16) :
    independentState (entangle phase state) = state := by
  rfl

theorem entangle_independent
    (phase : PhaseMap) (pair : Entangled phase) :
    entangle phase (independentState pair) = pair := by
  cases pair with
  | mk left right linked =>
      cases linked
      rfl

theorem right_view_is_determined
    (phase : PhaseMap) (pair : Entangled phase) :
    pair.right = phase.forward pair.left := by
  exact pair.linked

/- A local, import-free bijection certificate. -/
structure Bijection (α β : Type) where
  toFun : α → β
  invFun : β → α
  left_inv : ∀ x, invFun (toFun x) = x
  right_inv : ∀ y, toFun (invFun y) = y

/- Consequently, Entangled phase and Cubi16 carry the same independent data. -/
def entangledBijection (phase : PhaseMap) :
    Bijection Cubi16 (Entangled phase) where
  toFun := entangle phase
  invFun := independentState
  left_inv := independent_entangle phase
  right_inv := entangle_independent phase

/- -------------------------------------------------------------------------
   Phase tail: \ ~~~ \ ~~~ >>
   ------------------------------------------------------------------------- -/

inductive TailStep where
  | reverse
  | phase
  | forward
deriving DecidableEq, Repr

def phaseTail : List TailStep :=
  [ .reverse,
    .phase, .phase, .phase,
    .reverse,
    .phase, .phase, .phase,
    .forward, .forward ]

theorem phase_tail_length : phaseTail.length = 10 := by
  decide

/- -------------------------------------------------------------------------
   Spatial word: |||| /\\\\\\\\/ |||| /\\

   rails4  = ||||       (4)
   vortex6 = /\\\\\\\\/     (6)
   rails4  = ||||       (4)
   close2  = /\\         (2)
                         ----
                           16
   ------------------------------------------------------------------------- -/

inductive Stroke where
  | rail
  | slash
  | backslash
deriving DecidableEq, Repr

def rails4 : List Stroke :=
  [ .rail, .rail, .rail, .rail ]

def vortex6 : List Stroke :=
  [ .slash,
    .backslash, .backslash, .backslash, .backslash,
    .slash ]

def close2 : List Stroke :=
  [ .slash, .backslash ]

def spatialWord : List Stroke :=
  rails4 ++ vortex6 ++ rails4 ++ close2

theorem rails4_length : rails4.length = 4 := by
  decide

theorem vortex6_length : vortex6.length = 6 := by
  decide

theorem close2_length : close2.length = 2 := by
  decide

theorem full_cubi_has_sixteen_strokes : spatialWord.length = 16 := by
  decide

/- -------------------------------------------------------------------------
   Diodic fold: 16 -> 4 -> 2 -> 1 -> 0.
   ------------------------------------------------------------------------- -/

inductive FoldStage where
  | full16
  | cardinal4
  | entangled2
  | prime1
  | closed0
deriving DecidableEq, Repr

def stageValue : FoldStage → Nat
  | .full16 => 16
  | .cardinal4 => 4
  | .entangled2 => 2
  | .prime1 => 1
  | .closed0 => 0

def foldNext : FoldStage → Option FoldStage
  | .full16 => some .cardinal4
  | .cardinal4 => some .entangled2
  | .entangled2 => some .prime1
  | .prime1 => some .closed0
  | .closed0 => none

theorem exact_fold_chain :
    foldNext .full16 = some .cardinal4 ∧
    foldNext .cardinal4 = some .entangled2 ∧
    foldNext .entangled2 = some .prime1 ∧
    foldNext .prime1 = some .closed0 ∧
    foldNext .closed0 = none := by
  decide

theorem fold_is_diodic :
    foldNext .cardinal4 ≠ some .full16 ∧
    foldNext .entangled2 ≠ some .cardinal4 ∧
    foldNext .prime1 ≠ some .entangled2 ∧
    foldNext .closed0 ≠ some .prime1 := by
  decide

theorem fold_values_strictly_decrease :
    stageValue .cardinal4 < stageValue .full16 ∧
    stageValue .entangled2 < stageValue .cardinal4 ∧
    stageValue .prime1 < stageValue .entangled2 ∧
    stageValue .closed0 < stageValue .prime1 := by
  decide

/- -------------------------------------------------------------------------
   S.a.p.p.h.o.n: soft-closed execution with retained local state.
   ------------------------------------------------------------------------- -/

structure Sapphon where
  retained : Cubi16
  terminal : FoldStage
  terminal_is_zero : terminal = .closed0

def sealSapphon {phase : PhaseMap} (pair : Entangled phase) : Sapphon where
  retained := independentState pair
  terminal := .closed0
  terminal_is_zero := rfl

theorem sapphon_soft_closes
    {phase : PhaseMap} (pair : Entangled phase) :
    stageValue (sealSapphon pair).terminal = 0 := by
  rfl

theorem sapphon_retains_state
    (phase : PhaseMap) (state : Cubi16) :
    (sealSapphon (entangle phase state)).retained = state := by
  rfl

/- Compact certificate tying the frozen invariants together. -/
theorem quant_root_0_certificate :
    fuseWeight .half .half = some .full ∧
    ((4 * 4 : Nat) ^ 4) = 2 ^ 16 ∧
    phaseTail.length = 10 ∧
    spatialWord.length = 16 ∧
    foldNext .full16 = some .cardinal4 ∧
    foldNext .cardinal4 = some .entangled2 ∧
    foldNext .entangled2 = some .prime1 ∧
    foldNext .prime1 = some .closed0 ∧
    foldNext .closed0 = none := by
  decide

#eval canonicalName
#eval ((4 * 4 : Nat) ^ 4)
#eval phaseTail.length
#eval spatialWord.length

end QuantRoot0

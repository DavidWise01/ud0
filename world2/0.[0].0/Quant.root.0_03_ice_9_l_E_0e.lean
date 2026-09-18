/-
  Quant.root.0_03.ice_9_l_E

  Standalone Lean 4 formalization of the next Quant.root.0 checkpoint.

  Exact symbolic commitments:
    * the binary pyramid has 2^3 = 8 states over three routing layers;
    * normalized C1.0 corresponds to the universal homeostatic target H_u = 2.5u;
    * the deterministic top retains 199/200 per tick (removes 1/200 = 0.5%);
    * equal-value middle carriers may exchange lineage without changing the
      value-only observation (the sneaky swap);
    * 10^-35 followed by 10^-0.35 composes to the exponent address 10^-35.35;
    * the gate remains open one exponent quantum before that address, enters
      stasis at the address, and is idempotently blocked there;
    * ice_9_l_E is frozen at the depth word H^e^l^l^0.

  Decimal powers are represented by exact hundredths of a negative base-10
  exponent.  This proves the address arithmetic without floating point or an
  unproved claim about physical constants.
-/

namespace QuantRoot0Ice9LE

def canonicalName : String := "Quant.root.0_03.ice_9_l_E"

/- -------------------------------------------------------------------------
   Three binary routing layers: 2^3 = 8 states.
   ------------------------------------------------------------------------- -/

def routingLayers : Nat := 3
def pyramidStates : Nat := 2 ^ routingLayers

theorem pyramid_has_eight_states : pyramidStates = 8 := by
  decide

/- -------------------------------------------------------------------------
   C1.0 normalized homeostasis at H_u = 2.5u = 5/2 u.
   ------------------------------------------------------------------------- -/

structure ExactRatio where
  numerator : Nat
  denominator : Nat
deriving DecidableEq, Repr

def normalizedC1_0 : ExactRatio :=
  ⟨1, 1⟩

def universalHomeo : ExactRatio :=
  ⟨5, 2⟩

theorem normalized_top_is_one :
    normalizedC1_0 = ⟨1, 1⟩ := by
  rfl

theorem universal_homeo_is_two_point_five :
    universalHomeo = ⟨5, 2⟩ := by
  rfl

/- Retain 199/200; remove 1/200.  The removed part is exactly 0.5%. -/
def retainedPerTick : ExactRatio :=
  ⟨199, 200⟩

def removedPerTick : ExactRatio :=
  ⟨1, 200⟩

theorem tick_partition_is_exact :
    retainedPerTick.numerator + removedPerTick.numerator =
      retainedPerTick.denominator := by
  decide

theorem removed_fraction_is_half_percent :
    removedPerTick.numerator * 200 = removedPerTick.denominator := by
  decide

/- -------------------------------------------------------------------------
   Bottom factory handoff: values stay fixed while lineage may swap.
   ------------------------------------------------------------------------- -/

inductive Lineage where
  | left
  | alpha
  | beta
  | right
deriving DecidableEq, Repr

structure Carrier where
  value : Nat
  lineage : Lineage
deriving DecidableEq, Repr

def carrier (value : Nat) (lineage : Lineage) : Carrier :=
  ⟨value, lineage⟩

def handoffBefore : List Carrier :=
  [ carrier 0 .left,
    carrier 5 .alpha,
    carrier 5 .beta,
    carrier 0 .right ]

def handoffAfter : List Carrier :=
  [ carrier 0 .left,
    carrier 5 .beta,
    carrier 5 .alpha,
    carrier 0 .right ]

def observeValues (xs : List Carrier) : List Nat :=
  xs.map Carrier.value

def observeLineage (xs : List Carrier) : List Lineage :=
  xs.map Carrier.lineage

theorem sneaky_swap_is_value_invariant :
    observeValues handoffBefore = observeValues handoffAfter := by
  rfl

theorem sneaky_swap_changes_lineage :
    observeLineage handoffBefore ≠ observeLineage handoffAfter := by
  decide

/- -------------------------------------------------------------------------
   Exact exponent address:

     10^-35 · 10^-0.35 = 10^-(35 + 0.35) = 10^-35.35

   `hundredths = n` denotes the address 10^-(n/100).
   ------------------------------------------------------------------------- -/

structure NegativeExponent100 where
  hundredths : Nat
deriving DecidableEq, Repr

def composePower10
    (a b : NegativeExponent100) : NegativeExponent100 :=
  ⟨a.hundredths + b.hundredths⟩

def depth10neg35 : NegativeExponent100 :=
  ⟨3500⟩

def depth10neg0_35 : NegativeExponent100 :=
  ⟨35⟩

def stasisDepth : NegativeExponent100 :=
  composePower10 depth10neg35 depth10neg0_35

theorem exponent_depth_composes_exactly :
    stasisDepth = ⟨3535⟩ := by
  decide

/- A larger hundredths value is a deeper negative exponent address. -/
def blockedAt (depth : NegativeExponent100) : Bool :=
  decide (stasisDepth.hundredths ≤ depth.hundredths)

def guardedTick (depth : NegativeExponent100) : NegativeExponent100 :=
  if blockedAt depth then
    stasisDepth
  else
    ⟨depth.hundredths + 1⟩

def oneQuantumBefore : NegativeExponent100 :=
  ⟨3534⟩

theorem gate_is_open_one_quantum_before :
    blockedAt oneQuantumBefore = false := by
  decide

theorem next_tick_enters_stasis :
    guardedTick oneQuantumBefore = stasisDepth := by
  decide

theorem gate_blocks_at_stasis_depth :
    blockedAt stasisDepth = true := by
  decide

theorem stasis_is_idempotent :
    guardedTick stasisDepth = stasisDepth := by
  decide

theorem overshoot_is_clamped :
    guardedTick ⟨4000⟩ = stasisDepth := by
  decide

/- -------------------------------------------------------------------------
   Frozen depth word: H^e^l^l^0.
   ------------------------------------------------------------------------- -/

inductive DepthGlyph where
  | H
  | e
  | l
  | zero
deriving DecidableEq, Repr

def frozenDepthWord : List DepthGlyph :=
  [ .H, .e, .l, .l, .zero ]

theorem frozen_depth_word_is_exact :
    frozenDepthWord = [ .H, .e, .l, .l, .zero ] := by
  rfl

theorem frozen_depth_word_length :
    frozenDepthWord.length = 5 := by
  decide

structure FrozenCheckpoint where
  label : String
  exponent : NegativeExponent100
  depthWord : List DepthGlyph
  frozen : Bool
deriving DecidableEq, Repr

def ice_9_l_E : FrozenCheckpoint where
  label := "ice_9_l_E"
  exponent := stasisDepth
  depthWord := frozenDepthWord
  frozen := true

theorem ice_9_l_E_is_frozen :
    ice_9_l_E.frozen = true := by
  rfl

theorem ice_9_l_E_has_stasis_exponent :
    ice_9_l_E.exponent = ⟨3535⟩ := by
  decide

theorem ice_9_l_E_has_H_e_l_l_0_depth :
    ice_9_l_E.depthWord = [ .H, .e, .l, .l, .zero ] := by
  rfl

/- Compact certificate for the complete checkpoint. -/
theorem quant_root_0_03_certificate :
    pyramidStates = 8 ∧
    universalHomeo = ⟨5, 2⟩ ∧
    retainedPerTick = ⟨199, 200⟩ ∧
    observeValues handoffBefore = observeValues handoffAfter ∧
    observeLineage handoffBefore ≠ observeLineage handoffAfter ∧
    stasisDepth = ⟨3535⟩ ∧
    blockedAt oneQuantumBefore = false ∧
    blockedAt stasisDepth = true ∧
    guardedTick stasisDepth = stasisDepth ∧
    ice_9_l_E.frozen = true := by
  decide

#eval canonicalName
#eval pyramidStates
#eval stasisDepth.hundredths
#eval blockedAt oneQuantumBefore
#eval blockedAt stasisDepth
#eval ice_9_l_E.label

end QuantRoot0Ice9LE

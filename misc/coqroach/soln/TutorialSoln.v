From CTF Require Import Simpl.

Require Import Arith.
Require Import Bool.
Require Import List.
Require Import Nat.
Require Import Coq.Strings.String.
Require Import Coq.Program.Equality.

Open Scope bool_scope.
Open Scope list_scope.
Open Scope string_scope.
Open Scope nat_scope.

(* Here are the tutorial problem solutions for completeness *)
Theorem conj1: forall P Q: Prop,
  P /\ Q -> Q.
Proof.
  (* BABY 1 *)
  intros.
  destruct H.
  assumption.
Qed.

Theorem transitive: forall P Q R: Prop,
  (P -> Q) -> (Q -> R) -> P -> R.
Proof.
  (* BABY 2 *)
  intros.
  apply H0.
  apply H.
  apply H1.
Qed.

Theorem or_logic: forall A B C D: Prop,
  (A -> B) -> (C -> D) -> A \/ C -> B \/ D.
Proof.
  (* BABY 3 *)
  intros.
  destruct H1.
  - left. apply H. apply H1.
  - right. apply (H0 H1). (* short hand of applying *)
Qed.

Theorem not_true_is_false: forall b: bool,
  b <> true -> b = false.
Proof.
  (* BABY 4 *)
  intros. destruct b.
  (* two different ways to solve this. *)
  - (* exfalso. apply H. reflexivity. *) destruct H. reflexivity.
  - reflexivity.
Qed.

(* THEOREM 5 *)
Theorem mult_plus_distr_r : forall n m p : nat,
  (n + m) * p = (n * p) + (m * p).
Proof.
  intros n m p. induction p.
  (* p = 0 *)
  - intros. repeat rewrite (mult_comm _ 0). reflexivity.
  (* p != 0 *)
  - intros. repeat rewrite (mult_comm _ (S p)). simpl.
    repeat rewrite (mult_comm p). rewrite IHp.
    rewrite <- plus_assoc. rewrite (plus_comm m). rewrite (plus_comm m).
    repeat rewrite plus_assoc. reflexivity.
Qed.
(* END *)

(* THEOREM 6 *)
Theorem conv_ex1:
  [update (update (fun _ => 0) "a" 2) "b" 3,
   #23 #+ %"a" #* %"b" #- (#1 #== #0)] EXPR>> 29.
Proof.
  remember (update (update (fun _ => 0) "a" 2) "b" 3) as st.
  assert (st "a" = 2) as StA. { rewrite Heqst. reflexivity. }
  assert (st "b" = 3) as StB. { rewrite Heqst. reflexivity. }
  replace 29 with (29 - 0); [|reflexivity]. apply LoSub.
  (* 23 + a * b || 29 *)
  - replace 29 with (23 + 6); [|reflexivity]. apply LoPlus.
    (* 23 *)
    + apply LoNumb.
    (* a * b *)
    + replace 6 with (2 * 3); [|reflexivity]. apply LoMul.
      -- (* a *) rewrite <- StA. apply LoVar.
      -- (* b *) rewrite <- StB. apply LoVar.
  (* 1 == 0 || 0 *)
  - apply (LoEquF st (#1) (#0) 1 0).
    + (* 1 *) apply LoNumb.
    + (* 0 *) apply LoNumb.
    + (* 1 <> 0 *) intros Contra. discriminate Contra.
Qed.
(* END *)
Check conv_ex1:
  [update (update (fun _ => 0) "a" 2) "b" 3,
   #23 #+ %"a" #* %"b" #- (#1 #== #0)] EXPR>> 29.

(* THEOREM 7 *)
Theorem conv_ex2: forall st,
  [fun _ => 0,
    "a" #= #1 #+ #2 #;
    "b" #= %"a" #+ #1] STMT>> st -> st "b" = 4.
Proof.
  intros. inversion H. inversion S1. inversion S2. subst.
  inversion ConvE. inversion ConvE0. inversion ConvE1. inversion ConvE2.
  inversion ConvE3. inversion ConvE4. subst. compute. reflexivity.
Qed.
(* END *)

(* THEOREM 8 *)
Ltac next_with H := inversion H; subst; clear H; simpl in *.

Ltac unseq := repeat
  (match goal with
  | SH: [_, _ #; _] STMT>> _ |- _ => next_with SH
  end).

Ltac do_eval := repeat
  (match goal with
  | EH: [_, _] EXPR>> _ |- _ => next_with EH
  end).

Tactic Notation "next" constr(st') :=
  match goal with
  | SH: [_, _] STMT>> st' |- _ => next_with SH; do_eval
  end.

Theorem conv_ex3: forall st,
  [fun _ => 0,
    (* S1 *) "a" #= #2 #;
    (* S2 *) "b" #= #2 #;
    (* S3 *) "x" #= #0 #;
    (* S4 *) WHILE %"b" #!= #0 DO
    (* S5 *)   "x" #= %"x" #+ %"a" #;
    (* S6 *)   "b" #= %"b" #- #1
    DONE] STMT>> st -> st "x" = 4.
Proof.
  intros st Stmt.

  (* a = 2 *)
  unseq. next st'1.
  (* b = 2 *)
  next st'0.
  (* x = 0 *)
  next st'.
  (* while (b != 0) ... -> true (b = 2) *)
  next st. next st. clear Eq.
    (* x = x + a *)
    unseq. next st'0.
    (* b = b - 1 *)
    next st'.
  (* while (b != 0) ... -> true (b = 1) *)
  next st. next st. clear Eq.
    (* x = x + a *)
    unseq. next st'0.
    (* b = b - 1 *)
    next st'.
  (* while (b != 0) ... -> false (b = 0) *)
  next st. next st. compute in Eq. exfalso. auto. exfalso. auto.
  (* end *)
  next st. reflexivity.
Qed.
(* END *)

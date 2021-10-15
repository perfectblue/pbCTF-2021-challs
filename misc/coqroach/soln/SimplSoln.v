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

(* If we tried to directly prove this, we will inevitably run into a wall. *)

(* First we prove that expression operational semantics is a function
 * on any starting expression and state. *)
Lemma expr_total:
  forall st e, exists n, [st, e] EXPR>> n.
Proof.
  intros st e.
  induction e.
  - exists n. apply LoNumb.
  - exists (st v). apply LoVar.
  - destruct IHe1 as [n1 IHe1]. destruct IHe2 as [n2 IHe2].
    exists (n1 + n2). apply LoPlus. all: assumption.
  - destruct IHe1 as [n1 IHe1]. destruct IHe2 as [n2 IHe2].
    exists (n1 - n2). apply LoSub. all: assumption.
  - destruct IHe1 as [n1 IHe1]. destruct IHe2 as [n2 IHe2].
    exists (n1 * n2). apply LoMul. all: assumption.
  - destruct IHe1 as [n1 IHe1]. destruct IHe2 as [n2 IHe2].
    destruct (n1 =? n2) eqn: En12.
    + exists 1. apply Nat.eqb_eq in En12. apply (LoEquT _ _ _ n1 n2).
      all: assumption.
    + exists 0. apply Nat.eqb_neq in En12. apply (LoEquF _ _ _ n1 n2).
      all: assumption.
  - destruct IHe1 as [n1 IHe1]. destruct IHe2 as [n2 IHe2].
    destruct (n1 =? n2) eqn: En12.
    + exists 0. apply Nat.eqb_eq in En12. apply (LoNeqF _ _ _ n1 n2).
      all: assumption.
    + exists 1. apply Nat.eqb_neq in En12. apply (LoNeqT _ _ _ n1 n2).
      all: assumption.
Qed.

Theorem confidence_hides_imp_expr_attempt1:
  forall e l, confident_expr e l -> usesConfidentExpr e l = false.
Proof.
  induction e.
  (* number *)
  - intros l Conf. reflexivity.
  (* variable *)
  - intros l Conf. unfold confident_expr in Conf. simpl in *.
    destruct (l v) eqn: Contains.
    (* if v is a confident var we have to show that confidence proposition
     * no longer holds. *)
    + remember (fun v' => (if (v' =? v)%string then 1 else 0)) as st1.
      remember (fun (_: var) => 0) as st2.
      discriminate (Conf st1 st2 1 0).
      -- (* Show that the states are still conf equal *) unfold state_conf_eq.
         intros v_some. destruct (v_some =? v)%string eqn:Evv.
         ++ (* v is in l *) left. apply String.eqb_eq in Evv.
            rewrite Evv. assumption.
         ++ (* otherwise it is equal *) right. rewrite Heqst1. rewrite Heqst2.
            rewrite Evv. reflexivity.
      -- (* st1 v = 1 *) replace 1 with (st1 v). apply LoVar.
         rewrite Heqst1. rewrite -> String.eqb_refl. reflexivity.
      -- (* st2 v = 0 *) replace 0 with (st2 v). apply LoVar.
         rewrite Heqst2. reflexivity.
    (* otherewise it is trivial. *)
    + reflexivity.
  (* addition *)
  - intros l Conf. simpl. rewrite -> IHe1. rewrite -> IHe2. reflexivity.
    (* Prove that confidence in addition must imply the confidence of the
     * subexpressions *)
    unfold confident_expr in Conf. unfold confident_expr.
    intros st1 st2 n2_1 n2_2 Est1_st2 Conv2_st1 Conv2_st2.
    destruct (expr_total st1 e1) as [n1_1 Conv1_st1].
    destruct (expr_total st2 e1) as [n1_2 Conv1_st2].
    assert ([st1, e1 #+ e2]EXPR >> n1_1 + n2_1) as Conv12_st1.
    { apply LoPlus. all: assumption. }
    assert ([st2, e1 #+ e2]EXPR >> n1_2 + n2_2) as Conv12_st2.
    { apply LoPlus. all: assumption. }
    specialize (Conf st1 st2 (n1_1 + n2_1) (n1_2 + n2_2) Est1_st2
      Conv12_st1 Conv12_st2).
    (* can we prove that n1_1 = n1_2? *)
    admit. admit.
  (* subtraction *)
  - intros l Conf. simpl. rewrite -> IHe1. rewrite -> IHe2. reflexivity.
    (* Prove that confidence in subtraction must imply the confidence of the
     * subexpressions *)
    unfold confident_expr in Conf. unfold confident_expr.
    intros st1 st2 n2_1 n2_2 Est1_st2 Conv2_st1 Conv2_st2.
    destruct (expr_total st1 e1) as [n1_1 Conv1_st1].
    destruct (expr_total st2 e1) as [n1_2 Conv1_st2].
    assert ([st1, e1 #- e2]EXPR >> n1_1 - n2_1) as Conv12_st1.
    { apply LoSub. all: assumption. }
    assert ([st2, e1 #- e2]EXPR >> n1_2 - n2_2) as Conv12_st2.
    { apply LoSub. all: assumption. }
    specialize (Conf st1 st2 (n1_1 - n2_1) (n1_2 - n2_2) Est1_st2
      Conv12_st1 Conv12_st2).
    (* can we prove that n1_1 = n1_2? But even if we prove that,
     * that would not be sufficient evidence! *)

    (* stuck... *)
Abort.

(* ...in fact we can actually provide a counterexample for this *)
Lemma confidence_hides_imp_expr_counter:
  ~(forall e l, confident_expr e l -> usesConfidentExpr e l = false).
Proof.
  intros Contra.
  specialize (Contra (%"a" #- %"a") (fun v => (v =? "a")%string)).
  simpl in Contra.
  discriminate Contra.

  (* Prove that this expression maintains confidence *)
  unfold confident_expr. intros st1 st2 n1 n2 Est1_st2 Conv_st1 Conv_st2.
  inversion Conv_st1. inversion Conv_st2. inversion ConvE1.
  inversion ConvE2. inversion ConvE0. inversion ConvE3.
  rewrite Nat.sub_diag. rewrite Nat.sub_diag.
  reflexivity.
Qed.

(* So we cannot actually prove the theorems straightforward. We need to find a
 * way to prove False, and then imply these two theorems. Fortunately we have a
 * few axioms + admitted theorems lying around. We use the unsafe
 * stmt_induction admitted theorem. *)

(*** START SUBMISSION ***)

Fixpoint no_while_0 (s: stmt): bool :=
  match s with
  | sEmpty => true
  | _ #= _ => true
  | s1 #; s2 => no_while_0 s2
  | IIF _ THEN s1 ELSE _ ENDIF => no_while_0 s1
  | WHILE #0 DO _ DONE => false
  | WHILE _ DO _ DONE => true
  end.

Definition P_never_while_0 (s: stmt) := no_while_0 s = true.

Theorem never_while_0:
  forall s, P_never_while_0 s.
Proof.
  intros s. apply stmt_induction. all: unfold P_never_while_0.
  (* trivial cases *)
  - auto.
  - auto.
  - auto.
  - auto.
  (* for the while loop*)
  - intros. destruct cond.
    (* for #n *)
    + destruct n.
      (* when n = 0 *)
      -- simpl. (* this is obviously impossible... *)
         simpl in H0. (* but so is H0 (which was the generated bad inductive
                       * hyp0thesis). *)
         discriminate.
      (* when n <> 0 *)
      -- reflexivity.
    (* other cases are trivial *)
    + auto.
    + auto.
    + auto.
    + auto.
    + auto.
    + auto.
Qed.

(* This a very silly theorem because that isn't true *)
Theorem some_while_0:
  ~(forall s, P_never_while_0 s).
Proof.
  intros Contra.
  discriminate (Contra (WHILE #0 DO sEmpty DONE)).
Qed.

(* Now we are ready to prove falsehood *)
Theorem falsehood:
  False.
Proof.
  assert (forall P: Prop, P -> ~P -> False).
  { intros P HP HnP. apply HnP. apply HP. }
  apply (H (forall s, P_never_while_0 s)).
  apply never_while_0. apply some_while_0.
Qed.

(* A theory where everything is true *)
Corollary everything:
  forall (P: Prop), P.
Proof.
  exfalso. apply falsehood.
Qed.

(* Go back and now prove the original target *)
Theorem confidence_hides_imp_expr:
  forall e l, confident_expr e l -> usesConfidentExpr e l = false.
Proof. apply everything. Qed.

Theorem confidence_hides_imp_stmt:
  forall s, P_confidence_hides_imp_stmt s.
Proof. apply everything. Qed.

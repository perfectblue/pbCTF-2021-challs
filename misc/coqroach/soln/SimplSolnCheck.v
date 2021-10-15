From CTF Require Simpl.
From CTF Require SimplSoln.

Check SimplSoln.confidence_hides_imp_expr:
  forall e l, Simpl.confident_expr e l -> Simpl.usesConfidentExpr e l = false.

Check SimplSoln.confidence_hides_imp_stmt:
  forall s, Simpl.P_confidence_hides_imp_stmt s.


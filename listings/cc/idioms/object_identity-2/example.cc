VarDecl *VarDecl::getCanonicalDecl();

bool CapturedStmt::capturesVariable(const VarDecl *Var) const {
  for (const auto &I : captures()) {
    if (!I.capturesVariable() && !I.capturesVariableByCopy())
      continue;
    if (I.getCapturedVar()->getCanonicalDecl() == Var->getCanonicalDecl())
      return true;
  }

  return false;
}

#   The contents of this file are subject to the Mozilla Public License
#   Version 1.1 (the "License"); you may not use this file except in compliance
#   with the License. You may obtain a copy of the License at
#   http://www.mozilla.org/MPL/
#   
#   Software distributed under the License is distributed on an "AS IS" basis,
#   WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
#   for the specific language governing rights and limitations under the
#   License.
#   
#   The Original Code is the Open Orbit space flight simulator.
#   
#   The Initial Developer of the Original Code is Mattias Holm. Portions
#   created by the Initial Developer are Copyright (C) 2006,2009 the
#   Initial Developer. All Rights Reserved.
#   
#   Contributor(s):
#       Mattias Holm <mattias.holm(at)openorbit.org>.
#   
#   Alternatively, the contents of this file may be used under the terms of
#   either the GNU General Public License Version 2 or later (the "GPL"), or
#   the GNU Lesser General Public License Version 2.1 or later (the "LGPL"), in
#   which case the provisions of GPL or the LGPL License are applicable instead
#   of those above. If you wish to allow use of your version of this file only
#   under the terms of the GPL or the LGPL and not to allow others to use your
#   version of this file under the MPL, indicate your decision by deleting the
#   provisions above and replace  them with the notice and other provisions
#   required by the GPL or the LGPL.  If you do not delete the provisions
#   above, a recipient may use your version of this file under either the MPL,
#   the GPL or the LGPL."

import ooscript

print "Running post init script..."

def SemiMin(semiMaj, ecc):
  return math.sqrt(1.0 - ecc**2.0) * semiMaj


stars = ooscript.SkyDrawable("data/stars.csv")
sgr = ooscript.Scenegraph().new()
sgr.setBackground(stars)

ooscript.setSg(sgr)
osys = ooscript.OrbitSys().new(sgr, "data/solsystem.hrml")
ooscript.setOrbSys(osys)

cam = ooscript.FreeCam()
cam.setParams(sgr, sgr.getScene("Sol/Earth"), 6.37e6, 0.0, 0.0, 0.0, 1.0, 0.0)
sgr.setCam(cam)

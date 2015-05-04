#include "fragment.h"

Fset* Fragmentor::fragment(OBMol* pm_mol) {
	OBMol* pmol = pm_mol;
	if(!pmol) return NULL;
	fragset.clear();//needed because now only one instance of fp class
	ringset.clear();
 
	//identify fragments starting at every atom
	OBAtom *patom;
	vector<OBNodeBase*>::iterator i;
	for (patom = pmol->BeginAtom(i);patom;patom = pmol->NextAtom(i)) {
		if(patom->IsHydrogen()) continue;
		vector<int> curfrag;
		vector<int> levels(pmol->NumAtoms());
		getFragments(levels, curfrag, 1, patom, NULL);
	}

	//Ensure that each chemically identical fragment is present only in a single
	DoRings();
	DoReverses();

	return &fragset;
}

void Fragmentor::getFragments(vector<int> levels, vector<int> curfrag,
					int level, OBAtom* patom, OBBond* pbond) {
	//Recursive routine to analyse schemical structure and populate fragset and ringset
	//Hydrogens,charges(except dative bonds), spinMultiplicity ignored
	const int Max_Fragment_Size = 7;
	int bo=0;
	if(pbond) {
		bo = pbond->IsAromatic() ? 5 : pbond->GetBO();
	}

	curfrag.push_back(bo);
	curfrag.push_back(patom->GetAtomicNum());
	levels[patom->GetIdx()-1] = level;

	vector<OBEdgeBase*>::iterator itr;
	OBBond *pnewbond;
	for (pnewbond = patom->BeginBond(itr);pnewbond;pnewbond = patom->NextBond(itr)) {
		if(pnewbond==pbond) continue; //don't retrace steps
		OBAtom* pnxtat = pnewbond->GetNbrAtom(patom);
		if(pnxtat->IsHydrogen()) continue;

		int atlevel = levels[pnxtat->GetIdx()-1];
		//ring 
		if(atlevel) {
			if(atlevel==1) {
				//If complete ring (last bond is back to starting atom) add bond at front
				//and save in ringset
				curfrag[0] = bo;
				ringset.insert(curfrag);
			}
		}
		else { //no ring
			if(level<Max_Fragment_Size)
			{
				//Do the next atom; levels, curfrag are passed by value and hence copied
				getFragments(levels, curfrag, level+1, pnxtat, pnewbond);
			}
		}
	}

	//do not save C,N,O single atom fragments
	if(curfrag[0]==0 &&
		(level>1 || patom->GetAtomicNum()>8  || patom->GetAtomicNum()<6)) {
		fragset.insert(curfrag); //curfrag ignored if an identical fragment already present
	}
}

void Fragmentor::DoReverses()
{
	SetItr itr;
	for(itr=fragset.begin();itr!=fragset.end();)
	{
		//Reverse the order of the atoms, add the smallest fragment and remove the larger
		SetItr titr = itr++; //Ensure have valid next iterator in case current one is erased
		vector<int> t1(*titr); //temporary copy
		reverse(t1.begin()+1, t1.end()); //(leave 0 at front alone)
		if(t1!=*titr)
		{
			//Add the larger fragment and delete the smaller
			if(t1>*titr)
			{
				fragset.erase(titr);
				fragset.insert(t1);
			}
			else
				fragset.erase(t1);
		}
	}
}

void Fragmentor::DoRings()
{
	//For each complete ring fragment, find its largest chemically identical representation
	//by rotating and reversing, and insert into the main set of fragments
	SetItr itr;
	for(itr=ringset.begin();itr!=ringset.end();++itr) {
		vector<int> t1(*itr); //temporary copy
		vector<int> maxring(*itr); //the current largest vector
		unsigned int i;
		for(i=0;i<t1.size()/2;++i) {
			//rotate atoms in ring
			rotate(t1.begin(),t1.begin()+2,t1.end());
			if(t1>maxring)
				maxring=t1;

			//Add the non-ring form of all ring rotations
			int tmp = t1[0];
			t1[0] = 0;
			fragset.insert(t1);
			t1[0] = tmp;

			//reverse the direction around ring
			vector<int> t2(t1);
			reverse(t2.begin()+1, t2.end());
			if(t2>maxring)
				maxring=t2;
		}
		fragset.insert(maxring);
	}
}

unsigned int Fragmentor::CalcHash(const vector<int>& frag) {
	//Something like... whole of fragment treated as a binary number modulus 1021
	const int MODINT = 108; //2^32 % 1021
	unsigned int hash=0;
	for(unsigned i=0;i<frag.size();++i)
		hash= (hash*MODINT + (frag[i] % 1021)) % 1021;
	return hash;
}

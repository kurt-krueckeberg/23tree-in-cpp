
enum position_state { beg, middle, end }; // finite states of iterator.

position_state position;  // holds current state.


constructors()
{
 position = beg;
}

end_constructor()
{

 position = end;
}

iterator& iterator::operator++()
{
  if (position == end) {
      return *this;  
  } 

  position = // middle or end;
  //...

}
iterator& iterator::operator--()
{

  if (position == beg) {
      return *this;
  }     



  
  position = // middle or end;
  
   
}


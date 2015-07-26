  #include <stdlib.h>
  #include <unistd.h>
  #include <pthread.h>
  #include <math.h>

  #include "pagerank.h"  

  void pagerank(list* plist, int ncores, int npages, int nedges, double dampener)
  {  

    double initial_p_rank=0.0;
    double fixed=0.0;
    double sigma=0.0;
    double convergence=0.0;
    
    register double* result_old = (double*)malloc(npages*sizeof (double));
    register double* result_new = (double*)malloc(npages*sizeof (double));
    double* temp = (double*)malloc(npages*sizeof (double));  
  
    //initial page rank
    initial_p_rank = 1/(double)npages;      

    //initialize the default page rank
    for (int j=0;j<npages;j++)
      *(result_old+j)=initial_p_rank;
   
    //calculate the fixed term
    fixed =(1-dampener)/npages;     
    
    register int i;  
    int flag=1;
    do
    {    
      convergence=0.0;
      i=0;

      //traverse the passed list and calculate the page rank based on the given formula
      node* curr = plist->head;
      while (curr !=NULL)   
      {        

          //if current page has inlinks pages then do the calculation otherwise it would be 0
          if (curr->page->inlinks!=NULL)
          {
            node* curr_in_page = curr->page->inlinks->head;    
            
            while (curr_in_page != NULL)
            {
              
              node* temp2 = page_list_find(plist, curr_in_page->page->name);           
              sigma = sigma+result_old[temp2->page->index]/(double)(curr_in_page->page->noutlinks);              
              curr_in_page = curr_in_page->next;              
            }
          }
          else if (curr->page->inlinks==NULL)
          {          
            sigma=0;          
          }

          //calculate the result and save it in the result_new array for each page
          sigma = sigma * dampener;        
          result_new[i] = sigma+ fixed;    

          //set the right value to variables and move to next page on the list
          sigma = 0;        
          curr=curr->next;        
          i++;       
      }  

      //calculate the convergence
      for (register int j=0;j<npages;j++)
      {
        convergence += pow(fabs(result_new[j] - result_old[j]),2);     
      }
      
      convergence = sqrt(convergence);    
      
      //copy over the new page rank to old one for next round otherwise set the flag and exit the loop
      if (convergence>EPSILON)
      {    
        for (register int h=0;h<npages;h++)
          result_old[h] = result_new[h];
          flag =1;
      } 
      else 
        flag=0;   
        
    }while (flag); 

    //print out the results
    node* curr = plist->head;
    i=0;
    while(curr!=NULL)
    {    
        printf("%s %.4lf\n",curr->page->name,result_new[i++]);  
        curr=curr->next;
      }

    //free the memory 
    free(result_new);
    free(result_old);
    free(temp);
  }

  /* DO NOT MODIFY BELOW THIS POINT */
  int main(void)
  {
      /*
       * DO NOT MODIFY THE MAIN FUNCTION OR HEADER FILE
       */

      list* plist = NULL;

      double dampener;
      int ncores, npages, nedges;

      /* read the input into the appropriate variables and populate the list of pages */
      read_input(&plist, &ncores, &npages, &nedges, &dampener);    

      /* run pagerank and print the results */
      pagerank(plist, ncores, npages, nedges, dampener);

      /* clean up the memory used by the list of pages */
      page_list_destroy(plist);

      return 0;
  }
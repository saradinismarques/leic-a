package pt.ulisboa.tecnico.classes.student;

import io.grpc.ManagedChannel;
import io.grpc.ManagedChannelBuilder;

import java.util.Scanner;
import java.util.List;

public class Student {

  private String name;
  private String id;

  public Student(){
  }

  public Student(String name, String id){
    this.name = name;
    this.id = id;
  }

  public String getName() {
    return this.name;
  }

  public String getId() {
    return this.id;
  }

  public void setName(String name){
    this.name = name;
  }

  public void setId(String id){
    this.id = id;
  }

  public String toString(){
    return "- aluno" + id + " " + name + "\n";
  }

  public static void main(String[] args) {

    Scanner scanner = new Scanner(System.in);

    final String host = "localhost";
    final int port = 5000;

    String aluno = "aluno";
    String studentId = args[0];
    String name = "";
    
      int nArg = args.length;

      if (nArg == 1){
        System.out.println("Please enter a student ID and a name");
        return;
      }
      
      if (!studentId.substring(0,5).equals(aluno) ){
        System.out.println("Please enter a student 2");
        return;
      }
      Integer id = Integer.parseInt(studentId.substring(5,studentId.length()));
      if ( id < 0 || id > 9999 ){
        System.out.println("Please enter a valid studentId 3");
        return;
      }
      
      for ( int n = 1; n < nArg; n++){
        if ( n != 1)
          name += " " + args[n];
        else
          name += args[n];

      }

      if ( name.length() > 30 ){
        System.out.println("Please enter a valid studentId 4");
        return;
      }

      if (name.equals("")){
        System.out.println("Please enter a valid name 5");
        return;
      }
    
    ManagedChannel namingServerChannel = ManagedChannelBuilder.forAddress(host,port).usePlaintext().build();

    StudentFrontend pf = new StudentFrontend(namingServerChannel, name, studentId);
    pf.ScanInput();
  }


}

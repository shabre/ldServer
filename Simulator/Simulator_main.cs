class Simulator_main
{
        public static void Main(string[] args){
            ClientNetworking cn=new ClientNetworking();
            cn.connect();
            cn.frameSend();

    }
}
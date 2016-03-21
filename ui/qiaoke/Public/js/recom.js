$(document).ready(function() {
	$.ajax({
                url : 'http://www.baidu.com',
                data:{name:value},
                cache : false, 
                async : true,
                type : "GET",
                dataType : 'json/xml/html',
                success : function (result){
                    //do some
                    alert(result.data);
                }
            });
    $('#btnLoad1.button').click(function() {
        $('#header').html("正在加载...");
        //$('#load_content').load('http://jqueryui.com/', );
        $('#load_content').hide().load('http://www.baidu.com', function(responseText, textStatus, XMLHttpRequest) {
            //所有状态成功，执行此函数，显示数据  //textStatus四种状态 success、error、notmodified、timeout
            if (textStatus == "error") {
                var msg = "错误: ";
                $('#header').html(msg + XMLHttpRequest.status + " " + XMLHttpRequest.statusText);
            }
            else if (textStatus == "timeout") {
                var msg = "操时: ";
                $('#header').html(msg + XMLHttpRequest.status + " " + XMLHttpRequest.statusText);
            }
            else {
                $('#header').html("加载完成");
                $(this).fadeIn();
            }
        });
    });
});
